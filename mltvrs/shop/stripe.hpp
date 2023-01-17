#pragma once

#include <span>

#include <boost/beast.hpp>
#include <boost/static_string.hpp>

#include <fmt/format.h>

#include <cpprest/uri.h>

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

            //! API key type.
            enum class type {
                pub, //!< API public key.
                priv //!< API private key.
            };

            //! API key access mode.
            enum class mode {
                test, //!< Testing-only API mode.
                live  //!< Love deployment API mode.
            };

            /**
             * @brief Construct an API key in the given access mode with the given key string.
             *
             * @param k_type  The API key type this is.
             * @param op_mode The API access mode this key is for.
             * @param key     The RFC-4648 key string.
             *
             * @{
             */
            constexpr api_key(type k_type, mode op_mode, const rfc4648_literal_type& key) noexcept;
            constexpr api_key(type k_type, mode op_mode, const rfc4648_string_type& key) noexcept;
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
            [[nodiscard]] constexpr auto& key_type() const noexcept { return m_type; }
            [[nodiscard]] constexpr auto& key_type() noexcept { return m_type; }
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
            type                m_type;
            mode                m_mode;
    };

    /**
     * @brief A line item in a shopping cart.
     */
    class line_item
    {
        public:
            /**
             * @brief Construct a line item from its price ID and quantity.
             *
             * @param price_ident The item's price ID.
             * @param quant       The number of items in the cart.
             */
            line_item(std::string price_ident, unsigned quant) noexcept;

            /**
             * @name Properties
             *
             * @brief Access line item properties.
             *
             * @return Returns the requested line item property.
             *
             * @{
             */
            [[nodiscard]] auto& price_id() const noexcept { return m_price_id; }
            [[nodiscard]] auto& price_id() noexcept { return m_price_id; }
            [[nodiscard]] auto& quantity() const noexcept { return m_quantity; }
            [[nodiscard]] auto& quantity() noexcept { return m_quantity; }
            //! @}

            /**
             * @brief Compare whether two line items have the same price ID and quantity.
             *
             * @param lhs The left-hand-size line item to compare.
             * @param rhs The right-hand-size line item to compare.
             *
             * @return Returns the comparison result.
             */
            [[nodiscard]] friend bool
            operator==(const line_item& lhs, const line_item& rhs) noexcept = default;

        private:
            std::string m_price_id;
            unsigned    m_quantity;
    };

    class checkout_request
    {
        private:
            using line_items_storage = std::vector<line_item>;

        public:
            using value_type             = line_items_storage::value_type;
            using size_type              = line_items_storage::size_type;
            using difference_type        = line_items_storage::difference_type;
            using reference              = line_items_storage::reference;
            using const_reference        = line_items_storage::const_reference;
            using pointer                = line_items_storage::pointer;
            using const_pointer          = line_items_storage::const_pointer;
            using iterator               = line_items_storage::iterator;
            using const_iterator         = line_items_storage::const_iterator;
            using reverse_iterator       = line_items_storage::reverse_iterator;
            using const_reverse_iterator = line_items_storage::const_reverse_iterator;

            checkout_request(web::uri success, web::uri cancel, line_items_storage items) noexcept;

            [[nodiscard]] auto& success_url() const noexcept { return m_success_url; }
            [[nodiscard]] auto& success_url() noexcept { return m_success_url; }
            [[nodiscard]] auto& cancel_url() const noexcept { return m_cancel_url; }
            [[nodiscard]] auto& cancel_url() noexcept { return m_cancel_url; }
            [[nodiscard]] auto& line_items() const noexcept { return m_line_items; }
            [[nodiscard]] auto& line_items() noexcept { return m_line_items; }

            [[nodiscard]] auto& at(size_type pos) const { return m_line_items.at(pos); }
            [[nodiscard]] auto& at(size_type pos) { return m_line_items.at(pos); }
            [[nodiscard]] auto  operator[](size_type pos) const noexcept -> const_reference;
            [[nodiscard]] auto& operator[](size_type pos) noexcept { return m_line_items[pos]; }
            [[nodiscard]] auto& front() const noexcept { return m_line_items.front(); }
            [[nodiscard]] auto& front() noexcept { return m_line_items.front(); }
            [[nodiscard]] auto& back() const noexcept { return m_line_items.back(); }
            [[nodiscard]] auto& back() noexcept { return m_line_items.back(); }
            [[nodiscard]] auto  data() const noexcept { return m_line_items.data(); }
            [[nodiscard]] auto  data() noexcept { return m_line_items.data(); }

            [[nodiscard]] auto cbegin() const noexcept { return m_line_items.cbegin(); }
            [[nodiscard]] auto begin() const noexcept { return m_line_items.begin(); }
            [[nodiscard]] auto begin() noexcept { return m_line_items.begin(); }
            [[nodiscard]] auto cend() const noexcept { return m_line_items.cend(); }
            [[nodiscard]] auto end() const noexcept { return m_line_items.end(); }
            [[nodiscard]] auto end() noexcept { return m_line_items.end(); }
            [[nodiscard]] auto crbegin() const noexcept { return m_line_items.crbegin(); }
            [[nodiscard]] auto rbegin() const noexcept { return m_line_items.rbegin(); }
            [[nodiscard]] auto rbegin() noexcept { return m_line_items.rbegin(); }
            [[nodiscard]] auto crend() const noexcept { return m_line_items.crend(); }
            [[nodiscard]] auto rend() const noexcept { return m_line_items.rend(); }
            [[nodiscard]] auto rend() noexcept { return m_line_items.rend(); }

            [[nodiscard]] auto empty() const noexcept { return m_line_items.empty(); }
            [[nodiscard]] auto size() const noexcept { return m_line_items.size(); }
            [[nodiscard]] auto max_size() const noexcept { return m_line_items.max_size(); }
            void               reserve(size_type new_cap) { m_line_items.reserve(new_cap); }
            [[nodiscard]] auto capacity() const noexcept { return m_line_items.capacity(); }
            void               shrink_to_fit() { m_line_items.shrink_to_fit(); }

            void clear() noexcept { m_line_items.clear(); }
            auto insert(auto&&... insert_args) -> iterator;
            auto emplace(auto&&... emplace_args) -> iterator;
            auto erase(auto... erase_args) noexcept { return m_line_items.erase(erase_args...); }
            void push_back(auto&& value);
            void emplace_back(auto&&... emplace_back_args);
            void pop_back() noexcept { m_line_items.pop_back(); }
            void resize(auto&&... resize_args);
            void swap(checkout_request& other) noexcept;

            [[nodiscard]] friend bool
            operator==(const checkout_request& lhs, const checkout_request& rhs) noexcept = default;

        private:
            web::uri           m_success_url;
            web::uri           m_cancel_url;
            line_items_storage m_line_items;
    };

    namespace http {

        [[nodiscard]] auto make_request(const api_key& key, const auto& payload)
            -> boost::beast::http::request<boost::beast::http::string_body>;

    } // namespace http

} // namespace mltvrs::shop::stripe

#include <mltvrs/shop/ipp/stripe.ipp>
