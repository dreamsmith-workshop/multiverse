#include <boost/json.hpp>

#include <fmt/format.h>

#include <mltvrs/ietf/raw_encoding.hpp>
#include <mltvrs/shop/stripe.hpp>

namespace {

    namespace ranges = std::ranges;
    namespace json   = boost::json;

    [[nodiscard]] auto adjustable_quantity_to_json(mltvrs::interval<unsigned> configs)
    {
        if(configs == mltvrs::shop::stripe::line_item::default_adjustable_quantity) {
            return json::object{
                {"enabled", true}
            };
        }

        return json::object{
            {"enabled", true             },
            {"maximum", configs.max.value},
            {"minimum", configs.min.value}
        };
    }

    [[nodiscard]] auto items_to_json(const mltvrs::shop::stripe::checkout_request& request)
    {
        auto ret = json::array{};
        ret.reserve(request.size());
        ranges::transform(
            request,
            std::back_inserter(ret),
            [](const auto& item)
            {
                auto ret = json::object{
                    {"price",    item.price()   },
                    {"quantity", item.quantity()}
                };
                if(item.has_adjustable_quantity()) {
                    ret.insert({
                        {"adjustable_quantity",
                         adjustable_quantity_to_json(item.adjustable_quantity())}
                    });
                }

                return ret;
            });

        return ret;
    }

} // namespace

mltvrs::shop::stripe::line_item::line_item(
    std::string        price_id,
    unsigned           quant,
    interval<unsigned> adjust)
    : m_price{std::move(price_id)},
      m_quantity{quant},
      m_adjustable_quantity{adjust}
{
    if(quantity() > *m_adjustable_quantity) {
        throw std::invalid_argument{"given quantity is greater than the maximum quantity"};
    }
    if(quantity() < *m_adjustable_quantity) {
        throw std::invalid_argument{"given quantity is less than the minimum quantity"};
    }
}

mltvrs::shop::stripe::line_item::line_item(std::string price_id, unsigned quant) noexcept
    : m_price{std::move(price_id)},
      m_quantity{quant}
{
}

void mltvrs::shop::stripe::line_item::quantity(unsigned new_quant)
{
    if(new_quant > *m_adjustable_quantity) {
        throw std::out_of_range{"given quantity is greater than the maximum quantity"};
    }
    if(new_quant < *m_adjustable_quantity) {
        throw std::out_of_range{"given quantity is less than the minimum quantity"};
    }

    m_quantity = new_quant;
}

[[nodiscard]] bool mltvrs::shop::stripe::line_item::has_adjustable_quantity() const noexcept
{
    return m_adjustable_quantity.has_value();
}

[[nodiscard]] auto mltvrs::shop::stripe::line_item::if_adjustable_quantity() const noexcept
    -> const interval<unsigned>*
{
    return has_adjustable_quantity() ? std::addressof(*m_adjustable_quantity) : nullptr;
}

[[nodiscard]] auto mltvrs::shop::stripe::line_item::adjustable_quantity() const
    -> const interval<unsigned>&
{
    return m_adjustable_quantity.value();
}

void mltvrs::shop::stripe::line_item::adjustable_quantity(interval<unsigned> new_adj)
{
    if(quantity() > new_adj) {
        throw std::out_of_range{"existing quantity is greater than the new maximum quantity"};
    }
    if(quantity() < new_adj) {
        throw std::out_of_range{"existing quantity is less than the new minimum quantity"};
    }

    m_adjustable_quantity = new_adj;
}

mltvrs::shop::stripe::checkout_request::checkout_request(
    checkout_mode      mode,
    boost::url         success,
    boost::url         cancel,
    std::string        cust_ref_id,
    line_items_storage items) noexcept
    : m_mode{mode},
      m_success_url{std::move(success)},
      m_cancel_url{std::move(cancel)},
      m_client_reference_id{std::move(cust_ref_id)},
      m_line_items{std::move(items)}
{
}

[[nodiscard]] auto mltvrs::shop::stripe::checkout_request::client_reference_id() const noexcept
    -> const std::string&
{
    return m_client_reference_id;
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
    const boost::url& host,
    const boost::url& target,
    const api_key&    key,
    std::string       payload) -> boost::beast::http::request<boost::beast::http::string_body>
{
    namespace beast = boost::beast;

    auto ret =
        beast::http::request<beast::http::string_body>{beast::http::verb::get, target.buffer(), 20};
    ret.set(beast::http::field::host, host.buffer());
    ret.set(
        beast::http::field::authorization,
        fmt::format("Basic {}", ietf::encode_base64(key.full_string())));
    ret.body() = std::move(payload);

    return ret;
}

[[nodiscard]] auto mltvrs::shop::stripe::detail::serialize_payload(const checkout_request& request)
    -> std::string
{
    auto ret = json::object{
        {"success_url", request.success_url().buffer()},
        {"cancel_url",  request.cancel_url().buffer() },
        {"line_items",  items_to_json(request)        },
        {"mode",        "payment"                     }
    };
    if(!request.client_reference_id().empty()) {
        ret.insert({
            {"client_reference_id", request.client_reference_id()}
        });
    }

    return json::serialize(ret);
}
