#include <gsl/gsl>

template<typename T>
[[nodiscard]] constexpr auto mltvrs::interval<T>::cmp(T lhs, interval rhs) noexcept -> ordering
{
    Expects(rhs.max >= rhs.min);

    if((lhs > rhs.max) && (lhs > rhs.min)) {
        return ordering::greater;
    }
    if((lhs < rhs.max) && (lhs < rhs.min)) {
        return ordering::less;
    }
    if constexpr(std::same_as<ordering, std::partial_ordering>) {
        return ((lhs <= rhs.max) && (lhs >= rhs.min)) ? ordering::equivalent
                                                      : std::partial_ordering::unordered;
    } else {
        return ordering::equivalent;
    }
}

template<typename T>
[[nodiscard]] constexpr auto mltvrs::interval<T>::cmp(interval lhs, interval rhs) noexcept
    -> std::partial_ordering
{
    Expects(lhs.max >= lhs.min);
    Expects(rhs.max >= rhs.min);

    if(lhs == rhs) {
        return std::partial_ordering::equivalent;
    }
    if((lhs.max <= rhs.max) && (lhs.min < rhs.min)) {
        return std::partial_ordering::less;
    }
    if((lhs.max > rhs.max) && (lhs.min >= rhs.min)) {
        return std::partial_ordering::greater;
    }

    return std::partial_ordering::unordered;
}
