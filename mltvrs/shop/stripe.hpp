#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <boost/beast.hpp>
#include <boost/static_string.hpp>
#include <boost/url.hpp>

#include <mltvrs/interval.hpp>
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
            explicit constexpr api_key(
                type                        k_type,
                mode                        op_mode,
                const rfc4648_literal_type& key) noexcept;
            explicit constexpr api_key(
                type                       k_type,
                mode                       op_mode,
                const rfc4648_string_type& key) noexcept;
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
            //! Sentinel quantity adjustment configuration indicating no limits are specified.
            static constexpr auto default_adjustable_quantity =
                interval<unsigned>{.max = std::numeric_limits<unsigned>::max(), .min = 0};

            /**
             * @name Constructors
             *
             * @brief Construct a line item from its price ID and quantity.
             *
             * @param price_ident The item's price ID.
             * @param quant       The number of items in the cart.
             * @param adjust      The items' quantity adjustment configurations.
             *
             * @throw std::invalid_argument Given quantity is outside the given adjustment range.
             *
             * @{
             */
            explicit line_item(std::string price_id, unsigned quant, interval<unsigned> adjust);
            explicit line_item(std::string price_id, unsigned quant) noexcept;
            //! @}

            /**
             * @name Properties
             *
             * @brief Access line item properties.
             *
             * @return Returns the requested line item property.
             *
             * @throw std::out_of_range Item quantity is outside the quantity adjustment range.
             *
             * @{
             */
            [[nodiscard]] auto& price() const noexcept { return m_price; }
            [[nodiscard]] auto& price() noexcept { return m_price; }
            [[nodiscard]] auto& quantity() const noexcept { return m_quantity; }
            void                quantity(unsigned new_quant);
            [[nodiscard]] bool  has_adjustable_quantity() const noexcept;
            [[nodiscard]] auto if_adjustable_quantity() const noexcept -> const interval<unsigned>*;
            [[nodiscard]] auto adjustable_quantity() const -> const interval<unsigned>&;
            void               adjustable_quantity(interval<unsigned> new_adj);
            //! @}

        private:
            std::string                       m_price;
            unsigned                          m_quantity;
            std::optional<interval<unsigned>> m_adjustable_quantity = {};
    };

    /**
     * @brief Available modes of checking out a cart.
     */
    enum class checkout_mode {
        payment,     //!< One-time purchase.
        setup,       //!< Save payment details for later use.
        subscription //!< Set up fixed-price subscriptions.
    };

    /**
     * @brief A request to check out and pay for a shopping cart.
     *
     * A checkout request contains checkout result redirect information, plus the shopping cart to
     * perform checkout on.
     *
     * A checkout request satisfies the Standard requirements for
     * [Sequence Container](https://en.cppreference.com/w/cpp/named_req/SequenceContainer) and
     * [Contiguous Container](https://en.cppreference.com/w/cpp/named_req/ContiguousContainer) with
     * elements of type `stripe::line_item`, except that it does not provide any of the same
     * constructors.
     */
    class checkout_request
    {
        private:
            using line_items_storage = std::vector<line_item>;

        public:
            /**
             * @name Sequence Container Member Types
             *
             * @brief Types satisfying Standard requirements for a Sequence Container.
             *
             * @{
             */
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
            //! @}

            /**
             * @brief Create a checkout request with the given redirect links and cart items.
             *
             * @param mode        The mode of checkout to initiate.
             * @param success     The location for Stripe to direct a successful checkout to.
             * @param cancel      The location for Stripe to direct a canceled checkout to.
             * @param cust_ref_id The reference ID of the client initiating the checkout.
             * @param items       The cart items to checkout.
             */
            explicit checkout_request(
                checkout_mode      mode,
                boost::url         success,
                boost::url         cancel      = {},
                std::string        cust_ref_id = {},
                line_items_storage items       = {}) noexcept;

            /**
             * @name Properties
             *
             * @brief Access individual checkout request properties.
             *
             * @return Returns the requested checkout request property.
             *
             * @{
             */
            [[nodiscard]] auto& mode() const noexcept { return m_mode; }
            [[nodiscard]] auto& mode() noexcept { return m_mode; }
            [[nodiscard]] auto& success_url() const noexcept { return m_success_url; }
            [[nodiscard]] auto& success_url() noexcept { return m_success_url; }
            [[nodiscard]] auto& cancel_url() const noexcept { return m_cancel_url; }
            [[nodiscard]] auto& cancel_url() noexcept { return m_cancel_url; }
            [[nodiscard]] auto  client_reference_id() const noexcept -> const std::string&;
            [[nodiscard]] auto& client_reference_id() noexcept { return m_client_reference_id; }
            [[nodiscard]] auto& line_items() const noexcept { return m_line_items; }
            [[nodiscard]] auto& line_items() noexcept { return m_line_items; }
            //! @}

            /**
             * @name Element Access
             *
             * @brief Access individual line items in the shopping cart.
             *
             * @param pos The index of the line item to access.
             *
             * @return Returns the requested line item or items array.
             *
             * @{
             */
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
            //! @}

            /**
             * @name Iterators
             *
             * @brief Obtain an iterator into the line items in the cart.
             *
             * @return Returns the requested iterator.
             *
             * @{
             */
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
            //! @}

            /**
             * @name Capacity
             *
             * @brief Manage the cart line items count.
             *
             * @return Returns the requested cart count property.
             *
             * @{
             */
            [[nodiscard]] auto empty() const noexcept { return m_line_items.empty(); }
            [[nodiscard]] auto size() const noexcept { return m_line_items.size(); }
            [[nodiscard]] auto max_size() const noexcept { return m_line_items.max_size(); }
            void               reserve(size_type new_cap) { m_line_items.reserve(new_cap); }
            [[nodiscard]] auto capacity() const noexcept { return m_line_items.capacity(); }
            void               shrink_to_fit() { m_line_items.shrink_to_fit(); }
            //! @}

            /**
             * @name Modifiers
             *
             * @brief Modify the cart.
             *
             * @param insert_args       Standard Sequence Container insertion arguments.
             * @param emplace_args      Standard Sequence Container random emplacement arguments.
             * @param erase_args        Standard Sequence Container random erase arguments.
             * @param value             The line item to add to the cart.
             * @param emplace_back_args Standard Sequence Container back-emplacement arguments.
             * @param resize_args       Standard Sequence Container resize arguments.
             * @param other             The checkout request to swap with.
             *
             * @{
             */
            void clear() noexcept { m_line_items.clear(); }
            auto insert(auto&&... insert_args) -> iterator;
            auto emplace(auto&&... emplace_args) -> iterator;
            auto erase(auto... erase_args) noexcept { return m_line_items.erase(erase_args...); }
            void push_back(auto&& value);
            void emplace_back(auto&&... emplace_back_args);
            void pop_back() noexcept { m_line_items.pop_back(); }
            void resize(auto&&... resize_args);
            void swap(checkout_request& other) noexcept;
            //! @}

        private:
            checkout_mode      m_mode;
            boost::url         m_success_url;
            boost::url         m_cancel_url;
            std::string        m_client_reference_id;
            line_items_storage m_line_items;
    };

    namespace http {

        /**
         * @brief Convert a given payload into an HTTP request.
         *
         * @param key     The API key to use for the HTTP request.
         * @param payload The payload to convert into an HTTP request.
         *
         * @return Returns the built HTTP request.
         */
        [[nodiscard]] auto make_request(const api_key& key, const auto& payload)
            -> boost::beast::http::request<boost::beast::http::string_body>;

    } // namespace http

} // namespace mltvrs::shop::stripe

#include <mltvrs/shop/ipp/stripe.ipp>
