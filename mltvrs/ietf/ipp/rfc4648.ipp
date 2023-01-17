#include <cstddef>

#include <bit>

#include <gsl/gsl>

namespace mltvrs::ietf::detail {

    inline constexpr auto table =
        std::array{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                   'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

    [[nodiscard]] constexpr auto byte_triple_to_b6_groups(const byte_range auto& triple) noexcept
    {
        const auto bytes = std::ranges::views::transform(
            triple,
            [](auto raw) { return std::bit_cast<std::byte>(raw); });
        Ensures(std::ranges::size(bytes) == 3);

        return std::array{
            bytes[0] >> 2,
            ((bytes[0] << 4) & std::byte{0b0011'0000}) | (bytes[1] >> 4),
            ((bytes[1] << 2) & std::byte{0b0011'1100}) | (bytes[2] >> 6),
            bytes[2] & std::byte{0b0011'1111}};
    }

    [[nodiscard]] constexpr auto b6_groups_to_byte_triple(const byte_range auto& b6_groups) noexcept
    {
        Expects(std::ranges::size(b6_groups) == 4);

        return std::array{
            (b6_groups[0] << 2) | (b6_groups[1] >> 4),
            (b6_groups[1] << 4) | (b6_groups[2] >> 2),
            (b6_groups[2] << 6) | (b6_groups[3] & std::byte{0b0011'1111})};
    }

    [[nodiscard]] constexpr auto index_byte_to_base64_char(std::byte idx_byte) noexcept
    {
        const auto index = static_cast<std::size_t>(idx_byte);

        return table[index];
    }

    [[nodiscard]] constexpr auto base64_char_to_index_byte(char base64_char) noexcept
    {
        const auto index = std::distance(table.cbegin(), std::ranges::find(table, base64_char));

        return std::bit_cast<std::byte>(gsl::narrow<unsigned char>(index));
    }

} // namespace mltvrs::ietf::detail

template<std::convertible_to<std::string_view> Encoded>
[[nodiscard]] auto mltvrs::ietf::encode_base64(const byte_range auto& range) -> Encoded
{
    namespace ranges = std::ranges;

    constexpr auto src_stride = 3;

    if(ranges::empty(range)) {
        return "";
    }

    const auto src_size         = ranges::size(range);
    const auto src_full_triples = src_size / src_stride;
    const auto src_remainder    = src_size % src_stride;
    const auto num_chunks       = src_full_triples + (src_remainder ? 1 : 0);

    auto ret = Encoded{};
    ret.reserve(num_chunks * 4);
    for(auto begin = ranges::cbegin(range); std::distance(begin, ranges::cend(range)) >= src_stride;
        begin      += src_stride)
    {
        const auto triple   = ranges::subrange{begin, begin + src_stride};
        const auto b6_group = detail::byte_triple_to_b6_groups(triple);

        ranges::transform(b6_group, std::back_inserter(ret), detail::index_byte_to_base64_char);
    }
    if(src_remainder == 0) {
        return ret;
    }

    using range_type     = std::remove_cvref_t<decltype(range)>;
    using range_value_t  = ranges::range_value_t<range_type>;
    const auto remainder = std::array{
        range[src_full_triples * src_stride],
        (src_remainder == 2) ? range[src_full_triples * src_stride + 1] : range_value_t{},
        range_value_t{}};
    const auto b6_groups = detail::byte_triple_to_b6_groups(remainder);
    ranges::transform(b6_groups, std::back_inserter(ret), detail::index_byte_to_base64_char);
    switch(src_remainder) {
        case 1 :
            ret[ret.size() - 2] = '=';
            [[fallthrough]];
        case 2 :
            ret[ret.size() - 1] = '=';
            break;
        default :
            Ensures(false);
    }

    return ret;
}

template<mltvrs::ietf::byte_range Decoded = std::vector<std::byte>>
[[nodiscard]] auto mltvrs::ietf::decode_base64(std::string_view encoded) -> Decoded
{
    namespace ranges = std::ranges;
    namespace views  = ranges::views;

    constexpr auto src_stride = 4;
    constexpr auto dst_stride = 3;

    Expects(encoded.size() % src_stride == 0);

    if(encoded.empty()) {
        return {};
    }

    const auto num_chunks        = encoded.size() / src_stride;
    const auto dst_full_triplets = encoded.ends_with('=') ? (num_chunks - 1) : num_chunks;
    const auto dst_remainder     = encoded.ends_with("==") ? 1
                                 : encoded.ends_with('=')  ? 2
                                                           : dst_stride;

    using decoded_byte = ranges::range_value_t<Decoded>;
    auto ret           = Decoded{};
    ret.reserve(dst_stride * dst_full_triplets + dst_remainder);
    for(; encoded.size() > src_stride; encoded.remove_prefix(src_stride)) {
        const auto quad        = encoded.substr(0, src_stride);
        const auto b6_groups   = quad | views::transform(detail::base64_char_to_index_byte);
        const auto byte_triple = detail::b6_groups_to_byte_triple(b6_groups);

        ranges::transform(
            byte_triple,
            std::back_inserter(ret),
            [](auto byte) { return std::bit_cast<decoded_byte>(byte); });
    }

    const auto tail =
        encoded
        | views::transform([](auto b64_char) { return (b64_char == '=') ? char{} : b64_char; })
        | views::transform(detail::base64_char_to_index_byte);
    const auto remainder = detail::b6_groups_to_byte_triple(tail);
    for(auto i = 0; i < dst_remainder; ++i) {
        ret.push_back(std::bit_cast<decoded_byte>(remainder[i]));
    }

    return ret;
}
