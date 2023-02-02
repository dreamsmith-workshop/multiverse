#pragma once

#include <chrono>

namespace mltvrs::chrono {

    template<typename T>
    concept clock = std::chrono::is_clock_v<T>;

}; // namespace mltvrs::chrono
