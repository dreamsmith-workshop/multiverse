#include <random>
#include <ranges>

#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <boost/static_string.hpp>
#include <boost/url/src.hpp>

#include <mltvrs/ietf/raw_encoding.hpp>
#include <mltvrs/shop/stripe.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

namespace {

    namespace ranges = std::ranges;
    namespace beast  = boost::beast;
    namespace json   = boost::json;
    namespace ietf   = mltvrs::ietf;
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

        return table[dist(gen)];
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
    CATCH_GIVEN("success and failure, a client ID, and the line items")
    {
        const auto     success_url = boost::url{"https://example.com/success"};
        const auto     cancel_url  = boost::url{"https://example.com/cancel"};
        constexpr auto client      = "test_client";
        const auto     line_items  = std::vector{
            stripe::line_item{"first",  GENERATE(take(1, random(1u, 10u)))},
            stripe::line_item{"second", GENERATE(take(1, random(1u, 10u)))},
            stripe::line_item{"third",  GENERATE(take(1, random(1u, 10u)))}
        };

        const auto test_value = stripe::checkout_request{
            stripe::checkout_mode::payment,
            success_url,
            cancel_url,
            client,
            line_items};

        CATCH_THEN("constructing a payment session request preserves initializing values")
        {
            CATCH_REQUIRE(test_value.success_url() == success_url);
            CATCH_REQUIRE(test_value.cancel_url() == cancel_url);
            CATCH_REQUIRE(test_value.line_items().size() == line_items.size());
            for(auto i = 0; i < std::ssize(test_value.line_items()); ++i) {
                const auto& test_val = test_value.line_items().at(i);
                const auto& ref_val  = line_items.at(i);

                CATCH_REQUIRE(test_val.price() == ref_val.price());
                CATCH_REQUIRE(test_val.quantity() == ref_val.quantity());
                CATCH_REQUIRE(
                    test_val.has_adjustable_quantity() == ref_val.has_adjustable_quantity());
                if(test_val.has_adjustable_quantity()) {
                    CATCH_REQUIRE(test_val.adjustable_quantity() == ref_val.adjustable_quantity());
                }
            }
        }

        CATCH_AND_GIVEN("an API key")
        {
            const auto api_key = stripe::api_key{
                stripe::api_key::type::priv,
                stripe::api_key::mode::test,
                random_rfc4648<stripe::api_key::rfc4648_chars>()};

            CATCH_THEN("creating an HTTP request generates a properly-formed request")
            {
                const auto request = stripe::http::make_request(api_key, test_value);
                const auto api_url = boost::url{"https://api.stripe.com/v1/checkout/sessions"};

                const auto built_auth     = request.at(beast::http::field::authorization);
                const auto built_auth_b64 = built_auth.substr(sizeof("Basic ") - 1);
                const auto built_payload  = json::parse(request.body());
                const auto built_items    = line_items
                                       | ranges::views::transform(
                                             [](const auto& item)
                                             {
                                                 return json::object{
                                                     {"price",    item.price()   },
                                                     {"quantity", item.quantity()}
                                                 };
                                             });

                CATCH_REQUIRE(request.method() == beast::http::verb::get);
                CATCH_REQUIRE(request.target() == "/v1/checkout/sessions");
                CATCH_REQUIRE(request.version() == 20); // HTTP/2
                CATCH_REQUIRE(request.at(beast::http::field::host) == "api.stripe.com");
                CATCH_REQUIRE(built_auth.starts_with("Basic "));
                CATCH_REQUIRE(
                    std::string_view{api_key.full_string().c_str()}
                    == ietf::decode_base64<std::string>(built_auth_b64));
                CATCH_REQUIRE(
                    built_payload
                    == json::object{
                        {"success_url", success_url.buffer()},
                        {"cancel_url", cancel_url.buffer()},
                        {"client_reference_id", client},
                        {"line_items",
                         json::array(ranges::cbegin(built_items), ranges::cend(built_items))},
                        {"mode", "payment"}
                });
            }
        }
    }
}
