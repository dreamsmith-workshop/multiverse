#include <boost/json.hpp>

#include <mltvrs/ietf/rfc4648.hpp>
#include <mltvrs/shop/stripe.hpp>

namespace {

    namespace ranges = std::ranges;
    namespace json   = boost::json;

    [[nodiscard]] auto items_to_json(const mltvrs::shop::stripe::checkout_request& request)
    {
        auto ret = json::array{};
        ret.reserve(request.size());
        ranges::transform(
            request,
            std::back_inserter(ret),
            [](const auto& item)
            {
                return json::object{
                    {"price",    item.price_id()},
                    {"quantity", item.quantity()}
                };
            });

        return ret;
    }

} // namespace

mltvrs::shop::stripe::line_item::line_item(std::string price_ident, unsigned quant) noexcept
    : m_price_id{std::move(price_ident)},
      m_quantity{quant}
{
}

mltvrs::shop::stripe::checkout_request::checkout_request(
    web::uri           success,
    web::uri           cancel,
    line_items_storage items) noexcept
    : m_success_url{std::move(success)},
      m_cancel_url{std::move(cancel)},
      m_line_items{std::move(items)}
{
}

[[nodiscard]] auto mltvrs::shop::stripe::checkout_request::operator[](size_type pos) const noexcept
    -> const_reference
{
    return m_line_items[pos];
}

void mltvrs::shop::stripe::checkout_request::swap(checkout_request& other) noexcept
{
    std::swap(m_success_url, other.m_success_url);
    std::swap(m_cancel_url, other.m_cancel_url);
    std::swap(m_line_items, other.m_line_items);
}

[[nodiscard]] auto mltvrs::shop::stripe::detail::make_http_get(
    const web::uri& host,
    const web::uri& target,
    const api_key&  key,
    std::string     payload) -> boost::beast::http::request<boost::beast::http::string_body>
{
    namespace beast = boost::beast;

    auto ret = beast::http::request<beast::http::string_body>{
        beast::http::verb::get,
        target.to_string(),
        20};
    ret.set(beast::http::field::host, host.to_string());
    ret.set(
        beast::http::field::authorization,
        fmt::format("Basic {}", ietf::encode_base64(key.full_string())));
    ret.body() = std::move(payload);

    return ret;
}

[[nodiscard]] auto mltvrs::shop::stripe::detail::serialize_payload(const checkout_request& request)
    -> std::string
{
    return json::serialize(json::object{
        {"success_url", request.success_url().to_string()},
        {"cancel_url",  request.cancel_url().to_string() },
        {"line_items",  items_to_json(request)           },
        {"mode",        "payment"                        }
    });
}
