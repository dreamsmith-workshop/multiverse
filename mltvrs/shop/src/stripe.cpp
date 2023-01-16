#include <mltvrs/shop/stripe.hpp>

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
