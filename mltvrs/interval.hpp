#pragma once

#include <compare>

namespace mltvrs {

    enum class end_type {
        open,
        closed
    };

    template<typename T>
    struct end_point
    {
        public:
            constexpr end_point() = default;
            constexpr end_point(T val) noexcept : value{val} {}
            constexpr end_point(T val, end_type endt) noexcept : value{val}, type{endt} {}

            T        value;
            end_type type = end_type::closed;

            [[nodiscard]] friend constexpr bool
            operator==(const end_point& lhs, const end_point& rhs) noexcept = default;
            [[nodiscard]] friend constexpr auto
            operator<=>(const end_point& lhs, const end_point& rhs) noexcept -> std::weak_ordering
            {
                return lhs.value <=> rhs.value;
            }

            [[nodiscard]] friend constexpr bool operator==(T lhs, end_point rhs) noexcept
            {
                return (rhs.type == end_type::closed) && (lhs == rhs.value);
            }
            [[nodiscard]] friend constexpr auto operator<=>(T lhs, end_point rhs) noexcept
            {
                return lhs <=> rhs.value;
            }
    };

    template<typename T>
    struct interval
    {
        public:
            using ordering = std::conditional_t<
                std::three_way_comparable<T, std::weak_ordering>,
                std::weak_ordering,
                std::partial_ordering>;
            using end_point = mltvrs::end_point<T>;

            end_point max;
            end_point min;

            [[nodiscard]] friend constexpr bool
            operator==(const interval& lhs, const interval& rhs) noexcept = default;
            [[nodiscard]] friend constexpr auto
            operator<=>(const interval& lhs, const interval& rhs) noexcept -> std::partial_ordering
            {
                return cmp(lhs, rhs);
            }

            [[nodiscard]] friend constexpr auto operator<=>(T lhs, interval rhs) noexcept
                -> ordering
            {
                return cmp(lhs, rhs);
            }

        private:
            [[nodiscard]] static constexpr auto cmp(T lhs, interval rhs) noexcept -> ordering;
            [[nodiscard]] static constexpr auto cmp(interval lhs, interval rhs) noexcept
                -> std::partial_ordering;
    };

} // namespace mltvrs

#include <mltvrs/ipp/interval.ipp>
