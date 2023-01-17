#pragma once

#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace mltvrs::ietf {

    template<typename Range>
    concept byte_range = std::ranges::random_access_range<Range> &&
                         std::is_standard_layout_v<std::ranges::range_value_t<Range>> &&
                         (sizeof(std::ranges::range_value_t<Range>) == 1);

    template<std::convertible_to<std::string_view> Encoded = std::string>
    [[nodiscard]] auto encode_base64(const byte_range auto& range) -> Encoded;
    template<byte_range Decoded = std::vector<std::byte>>
    [[nodiscard]] auto decode_base64(std::string_view encoded) -> Decoded;

} // namespace mltvrs::ietf

#include <mltvrs/ietf/ipp/rfc4648.ipp>
