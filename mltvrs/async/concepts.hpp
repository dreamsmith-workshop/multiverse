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
        concept bool_or_void_or_coro_handle =
            std::same_as<T, bool> || std::same_as<T, void> || is_coroutine_handle_v<T>;

        template<typename T>
        concept awaiter =
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
        concept member_co_awaitable = awaiter<decltype(std::declval<T>().operator co_await())>;

        template<typename T>
        concept global_co_awaitable = awaiter<decltype(operator co_await(std::declval<T>()))>;

    } // namespace detail

    template<typename T>
    concept awaitable =
        detail::awaiter<T> || detail::member_co_awaitable<T> || detail::global_co_awaitable<T>;

} // namespace mltvrs::async
