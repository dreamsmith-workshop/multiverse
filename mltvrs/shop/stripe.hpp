#pragma once

#include <span>

#include <boost/static_string.hpp>

#include <mltvrs/string_literal.hpp>

namespace mltvrs::shop::stripe {

    /**
     * @brief A Stripe API key.
     */
    class api_key
    {
        public:
            static constexpr auto rfc4648_chars = 99; //!< Number of Stripe RFC-4648 key characters
            static constexpr auto prefix_chars  = 8;  //!< Number of Stripe key prefix characters

            //! Fixed-sized Stripe RFC-4648 key string literal, excluding the Stripe prefix.
            using rfc4648_literal_type = string_literal<rfc4648_chars>;
            //! Fixed-size Stripe RFC-4648 key string, excluding the Stripe prefix.
            using rfc4648_string_type  = boost::static_string<rfc4648_chars>;
            //! Stripe API key string, including the Stripe prefix.
            using full_string_type     = boost::static_string<rfc4648_chars + prefix_chars>;

            //! API key access mode.
            enum class mode {
                test, //!< Testing-only API mode.
                live  //!< Love deployment API mode.
            };

            /**
             * @brief Construct an API key in the given access mode with the given key string.
             *
             * @param op_mode The API access mode this key is for.
             * @param key     The RFC-4648 key string.
             *
             * @{
             */
            constexpr api_key(mode op_mode, const rfc4648_literal_type& key) noexcept;
            constexpr api_key(mode op_mode, const rfc4648_string_type& key) noexcept;
            //! @}

            /**
             * @name Properties
             *
             * @brief Access the individual properties of the API key.
             *
             * @return Returns the requested API key property value.
             *
             * @{
             */
            [[nodiscard]] constexpr auto& deploy_mode() const noexcept { return m_mode; }
            [[nodiscard]] constexpr auto& deploy_mode() noexcept { return m_mode; }
            [[nodiscard]] constexpr auto& key_digits() const noexcept { return m_key; }
            //! @}

            /**
             * @name Key Assignment
             *
             * @brief Assign a new key value.
             *
             * @param key The new RFC-4648 key string to assign to this object.
             *
             * @{
             */
            // NOLINTBEGIN(bugprone-exception-escape): string sizes fixed by design
            constexpr void assign_key(const rfc4648_literal_type& key) noexcept;
            constexpr void assign_key(const rfc4648_string_type& key) noexcept;
            //! @}

            /**
             * @brief Obtain the full API key string, including the Stripe prefix.
             *
             * @return The full Stripe API key string.
             */
            [[nodiscard]] constexpr auto full_string() const noexcept -> full_string_type;
            // NOLINTEND(bugprone-exception-escape)

        private:
            rfc4648_string_type m_key;
            mode                m_mode;
    };

} // namespace mltvrs::shop::stripe

#include <mltvrs/shop/ipp/stripe.ipp>
