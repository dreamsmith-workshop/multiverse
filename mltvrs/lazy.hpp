#pragma once

#include <coroutine>

namespace mltvrs {

    /**
     * @brief A lazy task coroutine that generates a single value.
     *
     * This template implements `std::lazy` from the ISO WG21 proposal
     * [`P1056R1`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1056r1.html).
     *
     * @tparam T The type of value the task lazily generates.
     */
    template<typename T>
    class [[nodiscard]] lazy
    {
        private:
            class awaitable_type;

        public:
            class promise_type;

            constexpr lazy(const lazy& rhs) = delete;
            constexpr lazy(lazy&& rhs) noexcept;

            constexpr auto operator=(const lazy& rhs) -> lazy& = delete;
            constexpr auto operator=(lazy&& rhs) -> lazy&      = delete;

            ~lazy() noexcept;

            constexpr auto operator co_await() const noexcept -> awaitable_type;

        private:
            class final_awaitable;

            explicit constexpr lazy(std::coroutine_handle<promise_type> coro) noexcept;

            std::coroutine_handle<promise_type> m_coroutine;
    };

} // namespace mltvrs

#include <mltvrs/ipp/lazy.ipp>
