#include <gsl/gsl>

constexpr mltvrs::shop::stripe::api_key::api_key(
    mode                        op_mode,
    const rfc4648_literal_type& key) noexcept
    : m_key{key.data(), key.size()},
      m_mode{op_mode}
{
    Ensures(m_key.size() == m_key.capacity());
}

constexpr mltvrs::shop::stripe::api_key::api_key(
    mode                       op_mode,
    const rfc4648_string_type& key) noexcept
    : m_key{key},
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
    auto ret =
        (deploy_mode() == mode::test) ? full_string_type{"sk_test_"} : full_string_type{"sk_live_"};
    ret += m_key;

    Ensures(ret.size() == ret.capacity());

    return ret;
}
