#include <gsl/gsl_assert>

#include <mltvrs/ranges.hpp>

template<typename Ref, typename V>
class mltvrs::async::generator<Ref, V>::iterator
{
    public:
        using value_type      = value;
        using difference_type = std::ptrdiff_t;

        constexpr iterator(iterator&& other) noexcept
            : m_coroutine{std::exchange(other.m_coroutine, {})}
        {
        }

        constexpr auto operator=(iterator&& other) noexcept -> iterator&
        {
            m_coroutine = std::exchange(other.m_coroutine, {});
            return *this;
        }

        [[nodiscard]] constexpr auto operator*() const
            noexcept(std::is_nothrow_copy_constructible_v<reference>) -> reference
        {
            return static_cast<reference>(*(m_coroutine.promise().m_value));
        }

        constexpr auto operator++() -> iterator&
        {
            m_coroutine.resume();
            return *this;
        }

        constexpr void operator++(int) { ++(*this); }

        [[nodiscard]] friend constexpr bool
        operator==(const iterator& itr, std::default_sentinel_t /* unused */)
        {
            return itr.m_coroutine.done();
        }

    private:
        friend class generator;

        explicit constexpr iterator(std::coroutine_handle<promise_type> coroutine) noexcept
            : m_coroutine{coroutine}
        {
        }

        std::coroutine_handle<promise_type> m_coroutine;
};

template<typename Ref, typename V>
class mltvrs::async::generator<Ref, V>::promise_type
{
    public:
        [[nodiscard]] constexpr auto get_return_object() noexcept
        {
            return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        constexpr auto initial_suspend() const noexcept { return std::suspend_always{}; }
        constexpr auto final_suspend() noexcept { return std::suspend_always{}; }

        constexpr auto yield_value(yielded val) noexcept
        {
            m_value = std::addressof(val);
            return std::suspend_always{};
        }

        constexpr void return_void() const noexcept {}
        constexpr void unhandled_exception() noexcept { m_except = std::current_exception(); }

    private:
        friend class iterator;

        std::add_pointer_t<yielded> m_value  = nullptr;
        std::exception_ptr          m_except = nullptr;
};

template<typename Ref, typename V>
constexpr mltvrs::async::generator<Ref, V>::generator(
    std::coroutine_handle<promise_type> coroutine) noexcept
    : m_coroutine{coroutine}
{
}

template<typename Ref, typename V>
constexpr mltvrs::async::generator<Ref, V>::generator(generator&& other) noexcept
    : m_coroutine{std::exchange(other.m_coroutine, {})}
{
}

template<typename Ref, typename V>
constexpr auto mltvrs::async::generator<Ref, V>::operator=(generator&& other) noexcept -> generator&
{
    if(this == std::addressof(other)) {
        return *this;
    }
    if(m_coroutine) {
        m_coroutine.destroy();
    }

    m_coroutine = std::exchange(other.m_coroutine, nullptr);

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
[[nodiscard]] constexpr auto mltvrs::async::generator<Ref, V>::begin() -> iterator
{
    m_coroutine.resume();
    return iterator{m_coroutine};
}
