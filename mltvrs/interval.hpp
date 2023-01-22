#pragma once

#include <compare>

namespace mltvrs {

    /**
     * @brief Types of mathematical interval endpoints.
     */
    enum class end_type {
        open,  //!< Open interval ending.
        closed //!< Closed interval ending.
    };

    /**
     * @brief An endpoint of a numeric interval.
     *
     * @tparam T The underlying numeric type.
     */
    template<typename T>
    struct end_point
    {
        public:
            //! The endpoint ordering type.
            using ordering = std::conditional_t<
                std::three_way_comparable<T, std::weak_ordering>,
                std::weak_ordering,
                std::partial_ordering>;

            /**
             * @name Constructors
             *
             * @brief Construct an endpoint at the given value.
             *
             * @param val  The endpoint value.
             * @param endt The endpoint type.
             *
             * @{
             */
            constexpr end_point() = default;
            constexpr end_point(T val) noexcept : value{val} {}
            constexpr end_point(T val, end_type endt) noexcept : value{val}, type{endt} {}
            //! @}

            T        value;                   //!< The value the endpoint resides at.
            end_type type = end_type::closed; //!< The type of interval endpoint.

            /**
             * @name Comparison
             *
             * @brief Compare two endpoints against each other.
             *
             * Endpoints have, at most, weak ordering by value. Endpoints compare equal if they
             * reside at the same numeric value, and also have the same endpoint type. Endpoints at
             * the same numeric value, but with different endpoint types, are equivalent but not
             * equal.
             *
             * Endpoint ordering can only be as strong as the underlying numeric type's ordering. If
             * two endpoints' numeric values are unordered with respect to each other, the endpoints
             * are also not ordered.
             *
             * Comparison against values of the underlying numeric type is supported via implicit
             * conversion from that type. Values of the underlying numeric type are considered
             * closed endpoints.
             *
             * @param lhs The left-hand-side endpoint to compare.
             * @param rhs The right-hand-side endpoint to compare.
             *
             * @return Returns the comparison result.
             *
             * @{
             */
            [[nodiscard]] friend constexpr bool
            operator==(const end_point& lhs, const end_point& rhs) noexcept = default;
            [[nodiscard]] friend constexpr auto
            operator<=>(const end_point& lhs, const end_point& rhs) noexcept -> ordering
            {
                return lhs.value <=> rhs.value;
            }
            //! @}
    };

    /**
     * @brief A numeric interval.
     *
     * The underlying numeric type must posess at least weak ordering.
     *
     * @tparam T The underlying numeric type.
     */
    template<typename T>
    struct interval
    {
        public:
            using end_point = mltvrs::end_point<T>; //!< The type of the interval endpoints.

            end_point max;                          //!< The high endpoint.
            end_point min;                          //!< The low endpoint.

            /**
             * @brief Check whether two intervals are exactly equal or not.
             *
             * Intervals only equal each other if their endpoints are exactly equal, meaning they
             * are at the same values, and have the same endpoint types.
             *
             * @param lhs The left-hand-side interval to compare.
             * @param rhs The right-hand-side interval to compare.
             *
             * @return Returns the comparison result.
             */
            [[nodiscard]] friend constexpr bool
            operator==(const interval& lhs, const interval& rhs) noexcept = default;

            /**
             * @brief Check whether a value is above or below an interval.
             *
             * A numeric value is partially ordered with respect to an interval. If the value is
             * above the interval, it compares greater than the interval. If the value is below the
             * interval, it compares less than the interval. If the value is part of the interval,
             * it is unordered with respect to the interval.
             *
             * @param lhs The numeric value to check against the interval.
             * @param rhs The interval to check the numeric value against.
             *
             * @return Returns the comparison result.
             */
            [[nodiscard]] friend constexpr auto operator<=>(T lhs, interval rhs) noexcept
                -> std::partial_ordering
            {
                return cmp(lhs, rhs);
            }

        private:
            [[nodiscard]] static constexpr auto cmp(T lhs, interval rhs) noexcept
                -> std::partial_ordering;
    };

} // namespace mltvrs

#include <mltvrs/ipp/interval.ipp>
