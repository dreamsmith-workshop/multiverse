#pragma once

#include <coroutine>

namespace mltvrs {

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
