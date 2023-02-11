#pragma once

#include <mltvrs/async/concepts.hpp>

namespace mltvrs::async {

    namespace detail {

        [[nodiscard]] decltype(auto) get_awaited_object(member_co_awaitable auto&& value) noexcept(
            noexcept(std::forward<decltype(value)>(value).operator co_await()))
        {
            return std::forward<decltype(value)>(value).operator co_await();
        }

        [[nodiscard]] decltype(auto) get_awaited_object(global_co_awaitable auto&& value) noexcept(
            noexcept(operator co_await(std::forward<decltype(value)>(value))))
        {
            return operator co_await(std::forward<decltype(value)>(value));
        }

        [[nodiscard]] decltype(auto) get_awaited_object(direct_co_awaitable auto&& value) noexcept
        {
            return std::forward<decltype(value)>(value);
        }

    } // namespace detail

    template<coroutine_promise P>
    using coroutine_t = decltype(std::declval<P>().get_return_object());
    template<coroutine C>
    using promise_type_t = detail::coro_prom_t<C>;

    template<awaitable T>
    using awaited_object_t = decltype(detail::get_awaited_object(std::declval<T>()));
    template<awaitable T>
    using await_result_t = decltype(std::declval<awaited_object_t<T>>().await_resume());

    template<coroutine Coroutine>
    struct coroutine_traits : public std::coroutine_traits<Coroutine>
    {
        public:
            using coroutine_type = Coroutine;
            using promise_type   = promise_type_t<coroutine_type>;

            [[nodiscard]] static constexpr auto handle_of(const coroutine_type& coro) noexcept
                -> std::coroutine_handle<promise_type>;
            [[nodiscard]] static constexpr auto handle_of(coroutine_type& coro) noexcept
                -> std::coroutine_handle<promise_type>;
    };

} // namespace mltvrs::async
