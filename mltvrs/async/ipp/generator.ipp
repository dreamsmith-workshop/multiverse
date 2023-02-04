#include <gsl/gsl_assert>

#include <mltvrs/ranges.hpp>

template<typename Ref, typename V>
class mltvrs::async::generator<Ref, V>::iterator
{
    public:
        using value_type      = value;
        using difference_type = std::ptrdiff_t;

        iterator(iterator&& other) noexcept : m_coroutine{std::exchange(other.m_coroutine, {})} {}

        auto operator=(iterator&& other) noexcept -> iterator&
        {
            m_coroutine = std::exchange(other.m_coroutine, {});
            return *this;
        }

        [[nodiscard]] auto operator*() const
            noexcept(std::is_nothrow_copy_constructible_v<reference>) -> reference
        {
            return static_cast<reference>(*(m_coroutine.promise().m_value));
        }

        auto operator++() -> iterator&
        {
            m_coroutine.resume();
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

        explicit iterator(std::coroutine_handle<promise_type> coroutine) noexcept
            : m_coroutine{coroutine}
        {
        }

        std::coroutine_handle<promise_type> m_coroutine;
};

template<typename Ref, typename V>
class mltvrs::async::generator<Ref, V>::promise_type
{
    public:
        [[nodiscard]] auto get_return_object() noexcept
        {
            return generator{
                std::coroutine_handle<promise_type>::from_promise(*this),
                std::make_unique<std::stack<std::coroutine_handle<>>>()};
        }

        auto initial_suspend() const noexcept { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }

        auto yield_value(yielded val) noexcept
        {
            m_value = std::addressof(val);
            return std::suspend_always{};
        }

        void return_void() const noexcept {}
        void unhandled_exception() noexcept { m_except = std::current_exception(); }

    private:
        friend class iterator;

        std::add_pointer_t<yielded> m_value  = nullptr;
        std::exception_ptr          m_except = nullptr;
};

template<typename Ref, typename V>
mltvrs::async::generator<Ref, V>::generator(
    std::coroutine_handle<promise_type>                  coro,
    std::unique_ptr<std::stack<std::coroutine_handle<>>> active) noexcept
    : m_coroutine{coro},
      m_active{std::move(active)}
{
}

template<typename Ref, typename V>
mltvrs::async::generator<Ref, V>::generator(generator&& other) noexcept
    : m_coroutine{std::exchange(other.m_coroutine, {})}
{
}

template<typename Ref, typename V>
auto mltvrs::async::generator<Ref, V>::operator=(generator other) noexcept -> generator&
{
    std::swap(m_coroutine, other.m_coroutine);
    std::swap(m_active, other.m_active);

    return *this;
}

template<typename Ref, typename V>
mltvrs::async::generator<Ref, V>::~generator() noexcept
{
    if(m_coroutine) {
        m_coroutine.destroy();
    }
}

template<typename Ref, typename V>
[[nodiscard]] auto mltvrs::async::generator<Ref, V>::begin() -> iterator
{
    m_coroutine.resume();
    return iterator{m_coroutine};
}
