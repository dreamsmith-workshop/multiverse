#include <gsl/gsl>

template<typename T>
[[nodiscard]] constexpr auto mltvrs::interval<T>::cmp(T lhs, interval rhs) noexcept
    -> std::partial_ordering
{
    Expects(rhs.max >= rhs.min);

    if((lhs > rhs.max) && (lhs > rhs.min)) {
        return ordering::greater;
    }
    if((lhs < rhs.max) && (lhs < rhs.min)) {
        return ordering::less;
    }

    return std::partial_ordering::unordered;
}
