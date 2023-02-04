#pragma once

#include <coroutine>
#include <ranges>

namespace mltvrs::async {

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
            constexpr generator(generator&& other) noexcept;

            auto           operator=(const generator& other) -> generator& = delete;
            constexpr auto operator=(generator&& other) noexcept -> generator&;

            ~generator() noexcept;

            [[nodiscard]] constexpr auto begin() -> iterator;
            [[nodiscard]] constexpr auto end() const noexcept { return std::default_sentinel_t{}; }

        private:
            explicit constexpr generator(std::coroutine_handle<promise_type> coroutine) noexcept;

            std::coroutine_handle<promise_type> m_coroutine = nullptr;
    };

} // namespace mltvrs::async

#include <mltvrs/async/ipp/generator.ipp>
