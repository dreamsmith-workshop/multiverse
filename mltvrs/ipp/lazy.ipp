#include <exception>
#include <stdexcept>
#include <utility>
#include <variant>

#include <gsl/gsl_assert>

#include <mltvrs/functional.hpp>

namespace mltvrs::detail {

    template<typename T>
    struct is_coroutine_handle : public std::false_type
    {
    };

    template<typename PromiseType>
    struct is_coroutine_handle<std::coroutine_handle<PromiseType>> : public std::true_type
    {
    };

    template<typename T>
    inline constexpr bool is_coroutine_handle_v = is_coroutine_handle<T>::value;

    template<typename T>
    concept coro_handle = is_coroutine_handle_v<T>;

    template<typename Derived, typename T>
    class promise_base
    {
        public:
            constexpr void return_value(std::convertible_to<T> auto&& retval)
            {
                static_cast<Derived*>(this)->do_return(std::forward<decltype(retval)>(retval));
            }
    };

    template<typename Derived>
    class promise_base<Derived, void>
    {
        public:
            constexpr void return_void() noexcept { static_cast<Derived*>(this)->do_return(); }
    };

    template<typename T>
    struct lazy_value
    {
        public:
            T value;

            [[nodiscard]] operator const T&() const& noexcept { return value; }
            [[nodiscard]] operator T&() & noexcept { return value; }
            [[nodiscard]] operator const T&&() const&& noexcept { return std::move(value); }
            [[nodiscard]] operator T&&() && noexcept { return std::move(value); }
    };

    template<>
    struct lazy_value<void>
    {
    };

    template<typename T>
    struct lazy_value<const T&>
    {
        public:
            std::reference_wrapper<const T> value;

            [[nodiscard]] operator const T&() const& noexcept { return value; }
            [[nodiscard]] operator const T&&() const&& noexcept { return std::move(value.get()); }
    };

    template<typename T>
    struct lazy_value<T&>
    {
        public:
            std::reference_wrapper<T> value;

            [[nodiscard]] operator T&() & noexcept { return value; }
            [[nodiscard]] operator T&&() && noexcept { return std::move(value.get()); }
    };

} // namespace mltvrs::detail

template<typename T>
class mltvrs::lazy<T>::final_awaitable
{
    public:
        const promise_type* prom;

        constexpr bool await_ready() const noexcept { return false; }

        constexpr auto await_suspend(detail::coro_handle auto /* awaiter */) const noexcept
        {
            return prom->continuation();
        }

        constexpr void await_resume() const noexcept {}
};

template<typename T>
class mltvrs::lazy<T>::promise_type : public detail::promise_base<promise_type, T>
{
    public:
        [[nodiscard]] constexpr auto get_return_object() noexcept
        {
            return lazy{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        constexpr auto initial_suspend() const { return std::suspend_always{}; }
        constexpr auto final_suspend() const noexcept { return final_awaitable{this}; }
        constexpr void unhandled_exception()
        {
            m_state.template emplace<std::exception_ptr>(std::current_exception());
        }

        constexpr void get() const
            requires(std::is_void_v<T>)
        {
            std::visit(
                overload{
                    [](std::monostate /* tag */) { Expects(false); },
                    [](std::exception_ptr exception) { std::rethrow_exception(exception); },
                    [](detail::lazy_value<T> /* value */) {}},
                m_state);
        }

        [[nodiscard]] constexpr auto get() -> T
            requires(!std::is_void_v<T>)
        {
            return std::visit(
                overload{
                    [](std::monostate /* tag */) -> T { Expects(false); },
                    [](std::exception_ptr exception) -> T { std::rethrow_exception(exception); },
                    [](detail::lazy_value<T>& value) -> T { return std::move(value); }},
                m_state);
        }

        [[nodiscard]] constexpr auto& continuation() noexcept { return m_continuation; }
        [[nodiscard]] constexpr auto& continuation() const noexcept { return m_continuation; }

    private:
        using state_type = std::variant<std::monostate, std::exception_ptr, detail::lazy_value<T>>;

        friend class detail::promise_base<promise_type, T>;

        constexpr void do_return(std::convertible_to<T> auto&& retval)
            requires(!std::is_void_v<T>)
        {
            m_state.template emplace<detail::lazy_value<T>>(std::forward<decltype(retval)>(retval));
        }

        constexpr void do_return() noexcept
            requires(std::is_void_v<T>)
        {
            m_state.template emplace<detail::lazy_value<T>>();
        }

        state_type              m_state        = {};
        std::coroutine_handle<> m_continuation = nullptr;
};

template<typename T>
class mltvrs::lazy<T>::awaitable_type
{
    public:
        explicit constexpr awaitable_type(std::coroutine_handle<promise_type> coro) noexcept
            : m_coroutine{coro}
        {
        }

        constexpr bool await_ready() const noexcept { return !m_coroutine || m_coroutine.done(); }

        constexpr auto await_suspend(std::coroutine_handle<> awaiter) noexcept
        {
            m_coroutine.promise().continuation() = awaiter;
            return m_coroutine;
        }

        constexpr auto await_resume() const noexcept -> T { return m_coroutine.promise().get(); }

    private:
        std::coroutine_handle<promise_type> m_coroutine;
};

template<typename T>
constexpr mltvrs::lazy<T>::lazy(std::coroutine_handle<promise_type> coro) noexcept
    : m_coroutine{coro}
{
}

template<typename T>
constexpr mltvrs::lazy<T>::lazy(lazy&& rhs) noexcept
    : m_coroutine{std::exchange(rhs.m_coroutine, nullptr)}
{
}

template<typename T>
mltvrs::lazy<T>::~lazy() noexcept
{
    if(m_coroutine) {
        m_coroutine.destroy();
    }
}

template<typename T>
constexpr auto mltvrs::lazy<T>::operator co_await() const noexcept -> awaitable_type
{
    return awaitable_type{m_coroutine};
}
