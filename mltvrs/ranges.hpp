#pragma once

#include <cstddef>

#include <memory>
#include <ranges>

namespace mltvrs::ranges {

    template<std::ranges::range R>
    struct elements_of
    {
        public:
            [[no_unique_address]] R range;
    };

    template<class R>
    elements_of(R&&) -> elements_of<R&&>;

} // namespace mltvrs::ranges
