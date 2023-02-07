#include <mltvrs/generator.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("iterating over a generator produces the correct results")
{
    CATCH_GIVEN("a generator that generates some series")
    {
        namespace ranges = std::ranges;
        namespace views  = ranges::views;

        const auto gen_coro =
            [](const ranges::input_range auto& input) -> mltvrs::ranges::generator<const int&>
        {
            for(const auto& elem : input) {
                co_yield elem;
            }
        };

        CATCH_WHEN("that generator is iterated over")
        {
            const auto data = GENERATE(chunk(10, take(10, random(-100, 100))));

            auto gen = gen_coro(data);
            auto ret = std::vector<int>{};
            for(const auto& elem : gen) {
                ret.push_back(elem);
            }

            CATCH_THEN("that generator produces the correct series")
            {
                CATCH_REQUIRE_THAT(ret, Catch::Matchers::Equals(data));
            }
        }

        CATCH_WHEN("that generator is iterated over recursively")
        {
            constexpr auto repeats = 3;

            const auto recurse_coro =
                [&](const ranges::input_range auto& input) -> mltvrs::ranges::generator<const int&>
            {
                for(auto i = 0; i < repeats; ++i) {
                    co_yield mltvrs::ranges::elements_of{gen_coro(input)};
                }
            };

            const auto data = GENERATE(chunk(10, take(10, random(-100, 100))));

            auto gen = recurse_coro(data);
            auto ret = std::vector<int>{};
            for(const auto& elem : gen) {
                ret.push_back(elem);
            }

            CATCH_THEN("that generator produces the correct series")
            {
                const auto first  = ret | views::take(data.size());
                const auto middle = ret | views::drop(data.size()) | views::take(data.size());
                const auto last   = ret | views::drop(2 * data.size());

                CATCH_REQUIRE_THAT(first, Catch::Matchers::RangeEquals(data));
                CATCH_REQUIRE_THAT(middle, Catch::Matchers::RangeEquals(data));
                CATCH_REQUIRE_THAT(last, Catch::Matchers::RangeEquals(data));
            }
        }
    }
}
