#pragma once

#include <mltvrs/async/concepts.hpp>

namespace mltvrs::async {

    namespace detail {

        [[nodiscard]] decltype(auto) get_awaiter(member_co_awaitable auto&& value) noexcept(
            noexcept(std::forward<decltype(value)>(value).operator co_await()))
        {
            return std::forward<decltype(value)>(value).operator co_await();
        }

        [[nodiscard]] decltype(auto) get_awaiter(global_co_awaitable auto&& value) noexcept(
            noexcept(operator co_await(std::forward<decltype(value)>(value))))
        {
            return operator co_await(std::forward<decltype(value)>(value));
        }

        [[nodiscard]] decltype(auto) get_awaiter(awaiter auto&& value) noexcept
        {
            return std::forward<decltype(value)>(value);
        }

    } // namespace detail

    template<typename T>
    using awaiter_t = decltype(detail::get_awaiter(std::declval<T>()));
    template<typename T>
    using await_result_t = decltype(std::declval<awaiter_t<T>>().await_resume());

} // namespace mltvrs::async
