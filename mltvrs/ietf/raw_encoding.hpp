#pragma once

#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace mltvrs::ietf {

    /**
     * @brief A range of raw bytes that can be encoded per RFC-4648.
     *
     * @tparam Range The type to check against this concept.
     */
    template<typename Range>
    concept byte_range = std::ranges::random_access_range<Range>
                      && std::is_standard_layout_v<std::ranges::range_value_t<Range>>
                      && (sizeof(std::ranges::range_value_t<Range>) == 1);

    /**
     * @brief Perform RFC-4648 base-64 encoding and decoding.
     *
     * @tparam Encoded The type to return the encoded string as.
     * @tparam Decode  The type to return the decoded bytes as.
     *
     * @param range   The range of bytes to encode.
     * @param encoded The base-64 string to decode.
     *
     * @return Returns the result of the base-64 conversion.
     *
     * @{
     */
    template<std::convertible_to<std::string_view> Encoded = std::string>
    [[nodiscard]] auto encode_base64(const byte_range auto& range) -> Encoded;
    template<byte_range Decoded = std::vector<std::byte>>
    [[nodiscard]] auto decode_base64(std::string_view encoded) -> Decoded;
    //! @}

} // namespace mltvrs::ietf

#include <mltvrs/ietf/ipp/raw_encoding.ipp>
