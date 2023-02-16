// clang-format off
#include <mltvrs/functional.hpp>
// clang-format on

#include <variant>

#include <catch2/catch_all.hpp>

CATCH_TEMPLATE_TEST_CASE(
    "invoking an overload set executes the expected overload",
    "[mltvrs][overload][p0051]",
    int,
    double)
{
    CATCH_GIVEN("an overload set")
    {
        auto       test_value = std::variant<std::monostate, int, double>{};
        const auto fun        = mltvrs::overload{
            [&](int arg) { test_value = arg; },
            [&](double arg) { test_value = arg; }};

        CATCH_WHEN("that overload set is invoked")
        {
            const auto data = GENERATE(take(
                5,
                random(
                    std::numeric_limits<TestType>::min(),
                    std::numeric_limits<TestType>::max())));

            fun(data);

            CATCH_THEN("the expected overload is executed")
            {
                CATCH_REQUIRE(std::holds_alternative<TestType>(test_value));
                CATCH_REQUIRE(std::get<TestType>(test_value) == data);
            }
        }
    }
}
