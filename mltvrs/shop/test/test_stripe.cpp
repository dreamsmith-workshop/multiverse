#include <random>

#include <boost/static_string.hpp>

#include <mltvrs/shop/stripe.hpp>

#include <catch2/catch_all.hpp>

namespace {

    namespace shop   = mltvrs::shop;
    namespace stripe = shop::stripe;

    [[nodiscard]] auto random_rfc4648_char() noexcept
    {
        constexpr auto table = std::array{
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

        static auto gen  = std::mt19937{};
        static auto dist = std::uniform_int_distribution<std::size_t>{0, table.size() - 1};

        return dist(gen);
    }

    template<std::size_t Length>
    [[nodiscard]] auto random_rfc4648()
    {
        auto ret = boost::static_string<Length>{};
        while(ret.size() != ret.capacity()) {
            ret.push_back(random_rfc4648_char());
        }

        return ret;
    }

} // namespace

CATCH_SCENARIO("payment session request message builds correctly")
{
    CATCH_GIVEN("success and failure URLs, and the line items")
    {
        const auto success_url = web::uri{"https://example.com/success"};
        const auto cancel_url  = web::uri{"https://example.com/cancel"};
        const auto line_items  = std::vector{
            stripe::line_item{ "first", GENERATE(take(1, random(1u, 10u)))},
            stripe::line_item{"second", GENERATE(take(1, random(1u, 10u)))},
            stripe::line_item{ "third", GENERATE(take(1, random(1u, 10u)))}
        };

        CATCH_THEN("constructing a payment session request preserves initializing values")
        {
            const auto test_value = stripe::checkout_request{success_url, cancel_url, line_items};

            CATCH_REQUIRE(test_value.success_url() == success_url);
            CATCH_REQUIRE(test_value.cancel_url() == cancel_url);
            CATCH_REQUIRE(test_value.line_items() == line_items);
        }

        CATCH_AND_GIVEN("an API key")
        {
            const auto api_key = stripe::api_key{
                stripe::api_key::type::priv,
                stripe::api_key::mode::test,
                random_rfc4648<stripe::api_key::rfc4648_chars>()};

            CATCH_THEN("creating an HTTP request generates a properly-formed request")
            {
            }
        }
    }
}
