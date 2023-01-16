#pragma once

#include <span>

#include <boost/static_string.hpp>

#include <mltvrs/string_literal.hpp>

namespace mltvrs::shop::stripe {

    class api_key
    {
        public:
            static constexpr auto rfc4648_chars = 99;
            static constexpr auto prefix_chars  = 8;

            using rfc4648_literal_type = string_literal<rfc4648_chars>;
            using rfc4648_string_type  = boost::static_string<rfc4648_chars>;
            using full_string_type     = boost::static_string<rfc4648_chars + prefix_chars>;

            enum class mode {
                test,
                live
            };

            constexpr api_key(mode op_mode, rfc4648_literal_type key) noexcept;
            constexpr api_key(mode op_mode, const rfc4648_string_type& key) noexcept;

            [[nodiscard]] constexpr auto deploy_mode() const noexcept { return m_mode; }
            constexpr void               deploy_mode(mode op_mode) noexcept { m_mode = op_mode; }
            // NOLINTBEGIN(bugprone-exception-escape): string sizes fixed by design
            [[nodiscard]] constexpr auto hex_digits() const noexcept -> rfc4648_string_type;
            constexpr void               hex_digits(rfc4648_literal_type key) noexcept;
            [[nodiscard]] constexpr auto full_string() const noexcept -> full_string_type;
            // NOLINTEND(bugprone-exception-escape)

        private:
            rfc4648_string_type m_key;
            mode                m_mode;
    };

} // namespace mltvrs::shop::stripe

#include <mltvrs/shop/ipp/stripe.ipp>
