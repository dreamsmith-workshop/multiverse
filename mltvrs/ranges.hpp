#pragma once

#include <cstddef>

#include <memory>
#include <ranges>

namespace mltvrs::ranges {

    /**
     * @brief Tag wrapper disambiguating when a range should be treated a sequence or single value.
     *
     * This template implements `std::ranges::elements_of` from the C++23 Standard, with the
     * below-noted deviations.
     *
     * @note This template does not support allocators.
     *
     * @tparam R The wrapped range type.
     */
    template<std::ranges::range R>
    struct elements_of
    {
        public:
            [[no_unique_address]] R range;
    };

    template<class R>
    elements_of(R&&) -> elements_of<R&&>;

} // namespace mltvrs::ranges
