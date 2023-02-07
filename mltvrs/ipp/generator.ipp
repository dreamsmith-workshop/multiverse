#include <variant>

#include <gsl/gsl_assert>

#include <mltvrs/ranges.hpp>

namespace mltvrs::detail {

    template<typename Yielded, typename PromiseType>
    struct suspend_const_lval
    {
        public:
            std::remove_cvref_t<Yielded> value;
            PromiseType*                 prom;

            constexpr bool await_ready() const noexcept { return false; }

            void await_suspend(std::coroutine_handle<> /* awaiter */) const noexcept
            {
                prom->m_value.ptr = std::addressof(value);
            }

            constexpr void await_resume() const noexcept {}
    };

    template<typename Yielded, typename PromiseType>
    struct suspend_mutable_lval
    {
        public:
            std::remove_cvref_t<Yielded> value;
            PromiseType*                 prom;

            constexpr bool await_ready() const noexcept { return false; }

            void await_suspend(std::coroutine_handle<> /* awaiter */) noexcept
            {
                prom->m_value.ptr = std::addressof(value);
            }

            constexpr void await_resume() const noexcept {}
    };

    template<typename Yielded>
    struct generator_value_ptr
    {
        public:
            std::variant<std::add_pointer_t<Yielded>, const generator_value_ptr*> ptr;
    };

    [[nodiscard]] constexpr auto* generator_value_deref(auto* value) noexcept
    {
        return value;
    }

    template<typename Yielded>
    [[nodiscard]] constexpr auto*
    generator_value_deref(const generator_value_ptr<Yielded>* pointer) noexcept
    {
        return std::visit([](const auto& ptr) { return generator_value_deref(ptr); }, pointer->ptr);
    }

} // namespace mltvrs::detail

template<typename Ref, typename V>
class mltvrs::generator<Ref, V>::iterator
{
    public:
        using value_type      = value;
        using difference_type = std::ptrdiff_t;

        iterator(iterator&& other) noexcept
            : m_coroutine{std::exchange(other.m_coroutine, {})},
              m_active{std::exchange(other.m_active, {})}
        {
        }

        auto operator=(iterator&& other) noexcept -> iterator&
        {
            m_coroutine = std::exchange(other.m_coroutine, {});
            m_active    = std::exchange(other.m_active, {});

            return *this;
        }

        [[nodiscard]] auto operator*() const
            noexcept(std::is_nothrow_copy_constructible_v<reference>) -> reference
        {
            return static_cast<reference>(*std::visit(
                [](const auto& ptr) { return detail::generator_value_deref(ptr); },
                m_coroutine.promise().m_value.ptr));
        }

        auto operator++() -> iterator&
        {
            m_active->top().resume();
            return *this;
        }

        void operator++(int) { ++(*this); }

        [[nodiscard]] friend bool
        operator==(const iterator& itr, std::default_sentinel_t /* unused */)
        {
            return itr.m_coroutine.done();
        }

    private:
        friend class generator;

        explicit iterator(
            std::coroutine_handle<promise_type>  coro,
            std::stack<std::coroutine_handle<>>* active) noexcept
            : m_coroutine{coro},
              m_active{active}
        {
        }

        std::coroutine_handle<promise_type>  m_coroutine;
        std::stack<std::coroutine_handle<>>* m_active;
};

template<typename Ref, typename V>
class mltvrs::generator<Ref, V>::promise_type
{
    public:
        [[nodiscard]] auto get_return_object() noexcept
        {
            auto active = std::make_unique<std::stack<std::coroutine_handle<>>>();
            m_active    = active.get();

            return generator{
                std::coroutine_handle<promise_type>::from_promise(*this),
                std::move(active)};
        }

        auto initial_suspend() const noexcept { return std::suspend_always{}; }
        auto final_suspend() noexcept
        {
            Expects(m_active->top() == std::coroutine_handle<promise_type>::from_promise(*this));

            struct do_continuation
            {
                public:
                    std::stack<std::coroutine_handle<>>* active;

                    constexpr bool await_ready() const noexcept { return false; }

                    void await_suspend(std::coroutine_handle<> /* awaiter */) const noexcept
                    {
                        active->pop();
                        if(active->empty()) {
                            return;
                        }

                        active->top().resume();
                    }

                    constexpr void await_resume() const noexcept {}
            };

            return do_continuation{.active = m_active};
        }

        auto yield_value(yielded val) noexcept
        {
            m_value.ptr = std::addressof(val);
            return std::suspend_always{};
        }

        auto yield_value(const std::remove_reference_t<yielded>& lval)
            requires(
                std::is_rvalue_reference_v<yielded>
                && std::constructible_from<
                    std::remove_cvref_t<yielded>,
                    const std::remove_reference_t<yielded>&>)
        {
            Expects(m_active->top() == std::coroutine_handle<promise_type>::from_promise(*this));

            using awaitable_type = std::conditional_t<
                std::is_const_v<yielded>,
                detail::suspend_const_lval<yielded, promise_type>,
                detail::suspend_mutable_lval<yielded, promise_type>>;

            return awaitable_type{.value = lval, .prom = this};
        }

        template<typename R2>
            requires(std::same_as<typename generator<R2>::yielded, yielded>)
        auto yield_value(ranges::elements_of<generator<R2>&&> gen) noexcept
        {
            Expects(m_active->top() == std::coroutine_handle<promise_type>::from_promise(*this));

            struct suspend_gen
            {
                public:
                    generator<R2> range;
                    promise_type* prom;

                    constexpr bool await_ready() const noexcept { return false; }

                    void await_suspend(std::coroutine_handle<> /* awaiter */)
                    {
                        prom->m_active->push(range.m_coroutine);
                        range.m_coroutine.resume();
                        prom->m_value.ptr = std::addressof(range.m_coroutine.promise().m_value);
                    }

                    void await_resume()
                    {
                        if(prom->m_except) {
                            std::rethrow_exception(prom->m_except);
                        }
                    }
            };

            gen.range.m_coroutine.promise().m_active = m_active;

            return suspend_gen{.range = std::move(gen.range), .prom = this};
        }

        template<std::ranges::input_range R>
            requires std::convertible_to<std::ranges::range_reference_t<R>, yielded>
        auto yield_value(ranges::elements_of<R> rng) noexcept
        {
            constexpr auto nested = [](std::ranges::iterator_t<R> itr,
                                       std::ranges::sentinel_t<R> sentinel)
                -> generator<yielded, std::ranges::range_value_t<R>>
            {
                for(; itr != sentinel; ++itr) {
                    co_yield static_cast<yielded>(*itr);
                }
            };

            return yield_value(ranges::elements_of(
                nested(std::ranges::begin(rng.range), std::ranges::end(rng.range))));
        }

        void await_transform() = delete;
        void return_void() const noexcept {}
        void unhandled_exception() noexcept
        {
            Expects(m_active->top() == std::coroutine_handle<promise_type>::from_promise(*this));
            m_except = std::current_exception();
        }

    private:
        friend class iterator;
        friend class detail::suspend_const_lval<yielded, promise_type>;
        friend class detail::suspend_mutable_lval<yielded, promise_type>;

        std::stack<std::coroutine_handle<>>* m_active = nullptr;
        detail::generator_value_ptr<yielded> m_value  = {};
        std::exception_ptr                   m_except = nullptr;
};

template<typename Ref, typename V>
mltvrs::generator<Ref, V>::generator(
    std::coroutine_handle<promise_type>                  coro,
    std::unique_ptr<std::stack<std::coroutine_handle<>>> active) noexcept
    : m_coroutine{coro},
      m_active{std::move(active)}
{
}

template<typename Ref, typename V>
mltvrs::generator<Ref, V>::generator(generator&& other) noexcept
    : m_coroutine{std::exchange(other.m_coroutine, {})}
{
}

template<typename Ref, typename V>
auto mltvrs::generator<Ref, V>::operator=(generator other) noexcept -> generator&
{
    std::swap(m_coroutine, other.m_coroutine);
    std::swap(m_active, other.m_active);

    return *this;
}

template<typename Ref, typename V>
mltvrs::generator<Ref, V>::~generator() noexcept
{
    if(m_coroutine) {
        m_coroutine.destroy();
    }
}

template<typename Ref, typename V>
[[nodiscard]] auto mltvrs::generator<Ref, V>::begin() -> iterator
{
    m_active->push(m_coroutine);
    m_coroutine.resume();

    return iterator{m_coroutine, m_active.get()};
}
