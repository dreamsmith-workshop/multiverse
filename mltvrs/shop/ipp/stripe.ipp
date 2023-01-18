#include <gsl/gsl>

namespace mltvrs::shop::stripe::detail {

    [[nodiscard]] auto make_http_get(
        const boost::url& host,
        const boost::url& target,
        const api_key&    key,
        std::string       payload) -> boost::beast::http::request<boost::beast::http::string_body>;
    [[nodiscard]] auto serialize_payload(const checkout_request& request) -> std::string;

} // namespace mltvrs::shop::stripe::detail

constexpr mltvrs::shop::stripe::api_key::api_key(
    type                        k_type,
    mode                        op_mode,
    const rfc4648_literal_type& key) noexcept
    : m_key{key.data(), key.size()},
      m_type{k_type},
      m_mode{op_mode}
{
    Ensures(m_key.size() == m_key.capacity());
}

constexpr mltvrs::shop::stripe::api_key::api_key(
    type                       k_type,
    mode                       op_mode,
    const rfc4648_string_type& key) noexcept
    : m_key{key},
      m_type{k_type},
      m_mode{op_mode}
{
    Ensures(m_key.size() == m_key.capacity());
}

constexpr void mltvrs::shop::stripe::api_key::assign_key(const rfc4648_literal_type& key) noexcept
{
    m_key.assign(key.data(), key.size());
}

constexpr void mltvrs::shop::stripe::api_key::assign_key(const rfc4648_string_type& key) noexcept
{
    Expects(key.size() == m_key.capacity());
    m_key = key;
}

[[nodiscard]] constexpr auto mltvrs::shop::stripe::api_key::full_string() const noexcept
    -> full_string_type
{
    auto ret  = (key_type() == type::pub) ? full_string_type{"pk_"} : full_string_type{"sk_"};
    ret      += (deploy_mode() == mode::test) ? "test_" : "live_";
    ret      += m_key;

    Ensures(ret.size() == ret.capacity());

    return ret;
}

auto mltvrs::shop::stripe::checkout_request::insert(auto&&... insert_args) -> iterator
{
    return m_line_items.insert(std::forward<decltype(insert_args)>(insert_args)...);
}

auto mltvrs::shop::stripe::checkout_request::emplace(auto&&... emplace_args) -> iterator
{
    return m_line_items.emplace(std::forward<decltype(emplace_args)>(emplace_args)...);
}

void mltvrs::shop::stripe::checkout_request::push_back(auto&& value)
{
    return m_line_items.push_back(std::forward<decltype(value)>(value));
}

void mltvrs::shop::stripe::checkout_request::emplace_back(auto&&... emplace_back_args)
{
    return m_line_items.emplace_back(
        std::forward<decltype(emplace_back_args)>(emplace_back_args)...);
}

void mltvrs::shop::stripe::checkout_request::resize(auto&&... resize_args)
{
    return m_line_items.resize(std::forward<decltype(resize_args)>(resize_args)...);
}

[[nodiscard]] auto mltvrs::shop::stripe::http::make_request(const api_key& key, const auto& payload)
    -> boost::beast::http::request<boost::beast::http::string_body>
{
    using payload_type = std::remove_cvref_t<decltype(payload)>;

    if constexpr(std::same_as<payload_type, checkout_request>) {
        return detail::make_http_get(
            boost::url{"api.stripe.com"},
            boost::url{"/v1/checkout/sessions"},
            key,
            detail::serialize_payload(payload));
    } else {
        // the condition cannot be simply `false` as that fails to compile, so make it impossible
        static_assert(std::same_as<payload_type, checkout_request>, "unhandled payload type");
    }
}
