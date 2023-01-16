#include <gsl/gsl>

constexpr mltvrs::shop::stripe::api_key::api_key(
    type                        k_type,
    mode                        op_mode,
    const rfc4648_literal_type& key) noexcept
    : m_type{k_type},
      m_key{key.data(), key.size()},
      m_mode{op_mode}
{
    Ensures(m_key.size() == m_key.capacity());
}

constexpr mltvrs::shop::stripe::api_key::api_key(
    type                       k_type,
    mode                       op_mode,
    const rfc4648_string_type& key) noexcept
    : m_type{k_type},
      m_key{key},
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
    auto ret = (key_type() == type::pub) ? full_string_type{"pk_"} : full_string_type{"sk_"};
    ret      += (deploy_mode() == mode::test) ? "test_" : "live_";
    ret      += m_key;

    Ensures(ret.size() == ret.capacity());

    return ret;
}
