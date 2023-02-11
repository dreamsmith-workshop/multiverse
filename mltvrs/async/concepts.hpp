#pragma once

#include <coroutine>

#include <boost/asio/execution/executor.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/is_executor.hpp>

namespace mltvrs::async {

    /**
     * @brief An Asio-compatible executor.
     *
     * See
     * [Boost.Asio](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio/reference/Executor1.html)
     * documentation for both Networking TS and Standard executor requirements.
     *
     * @tparam T The type to check against this concept.
     */
    template<typename T>
    concept executor =
        boost::asio::is_executor<T>::value || boost::asio::execution::is_executor_v<T>;

    /**
     * @brief An Asio-style execution context.
     *
     * See
     * [Boost.Asio](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio/reference/ExecutionContext.html)
     * documentation for execution context requirements.
     *
     * @tparam T The type to check against this concept.
     */
    template<typename T>
    concept execution_context = std::derived_from<T, boost::asio::execution_context>
                             && executor<typename T::executor_type> &&
                                // clang-format off
        requires(T ctx) {
            { ctx.get_executor() } -> std::same_as<typename T::executor_type>;
        }; // clang-format on

    namespace detail {

        struct any_type
        {
        };

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

        template<typename T>
        concept bool_or_void_or_coro_handle =
            std::same_as<T, bool> || std::same_as<T, void> || coro_handle<T>;

        template<typename T>
        concept direct_co_awaitable =
            requires(T& mref, const T& cref, T&& rref, std::coroutine_handle<> awaiter) {
                // clang-format off
                { cref.await_ready() }          -> std::same_as<bool>;
                { rref.await_ready() }          -> std::same_as<bool>;
                { mref.await_suspend(awaiter) } -> bool_or_void_or_coro_handle<>;
                { rref.await_suspend(awaiter) } -> bool_or_void_or_coro_handle<>;
                { mref.await_resume() };
                { rref.await_resume() };
                // clang-format on
            };

        template<typename T>
        concept member_co_awaitable =
            direct_co_awaitable<decltype(std::declval<T>().operator co_await())>;

        template<typename T>
        concept global_co_awaitable =
            direct_co_awaitable<decltype(operator co_await(std::declval<T>()))>;

        template<typename Promise, typename Return>
        concept returns_void = std::is_void_v<Return> &&
                               // clang-format off
            requires(Promise prom) {
                { prom.return_void() };
            }; // clang-format on

        template<typename Promise, typename Return>
        concept returns_value = std::same_as<Return, any_type> ||
                                // clang-format off
            requires(Promise prom, Return&& ret) {
                { prom.return_value(std::forward<Return>(ret)) };
            }; // clang-format on

        template<typename Promise, typename Return>
        concept has_return_member = returns_void<Promise, Return> || returns_value<Promise, Return>;

        template<typename T>
        using coro_prom_t = typename std::coroutine_traits<T>::promise_type;

        template<typename Coroutine, typename Promise>
        concept coro_for = std::same_as<Promise, coro_prom_t<Coroutine>>;

        template<typename Coroutine, typename Promise>
        concept any_coro_for =
            std::same_as<Coroutine, any_type>
            && coro_for<decltype(std::declval<Promise>().get_return_object()), Promise>;

        template<typename T, typename Promise, typename Coroutine>
        concept coroutine_for = any_coro_for<Coroutine, Coroutine> || coro_for<Coroutine, Promise>;

    } // namespace detail

    template<typename T>
    concept awaitable = detail::direct_co_awaitable<T> || detail::member_co_awaitable<T>
                     || detail::global_co_awaitable<T>;

    template<typename T, typename Return = detail::any_type, typename Coroutine = detail::any_type>
    concept coroutine_promise = detail::has_return_member<T, Return> &&
                                // clang-format off
        requires(T promise) {
            { promise.initial_suspend() }     -> awaitable<>;
            { promise.final_suspend() }       -> awaitable<>;
            { promise.unhandled_exception() };
            { promise.get_return_object() }   -> detail::coroutine_for<T, Coroutine>;
        }; // clang-format on

    template<typename T, typename Return = detail::any_type>
    concept coroutine = coroutine_promise<
        detail::coro_prom_t<std::remove_cvref_t<T>>,
        Return,
        std::remove_cvref_t<T>>;

    template<coroutine Coroutine>
    struct coroutine_traits;

    template<typename T>
    concept executable =
        detail::coro_handle<decltype(coroutine_traits<std::remove_cvref_t<T>>::handle_of(
            std::declval<T>()))>
        || awaitable<std::remove_cvref_t<T>>;

} // namespace mltvrs::async
