#pragma once

#include <coroutine>
#include <memory>
#include <ranges>
#include <stack>

namespace mltvrs {

    template<typename Ref, typename V = void>
    class generator : public std::ranges::view_interface<generator<Ref, V>>
    {
        private:
            using value     = std::conditional_t<std::is_void_v<V>, std::remove_cvref_t<Ref>, V>;
            using reference = std::conditional_t<std::is_void_v<V>, Ref&&, Ref>;

            class iterator;

        public:
            using yielded =
                std::conditional_t<std::is_reference_v<reference>, reference, const reference&>;

            class promise_type;

            generator(const generator&) = delete;
            generator(generator&& other) noexcept;

            auto operator=(generator other) noexcept -> generator&;

            ~generator() noexcept;

            [[nodiscard]] auto begin() -> iterator;
            [[nodiscard]] auto end() const noexcept { return std::default_sentinel_t{}; }

        private:
            explicit generator(
                std::coroutine_handle<promise_type>                  coro,
                std::unique_ptr<std::stack<std::coroutine_handle<>>> active) noexcept;

            std::coroutine_handle<promise_type>                  m_coroutine = nullptr;
            std::unique_ptr<std::stack<std::coroutine_handle<>>> m_active;
    };

} // namespace mltvrs

#include <mltvrs/ipp/generator.ipp>
