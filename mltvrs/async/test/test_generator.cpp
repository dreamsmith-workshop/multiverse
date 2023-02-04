#include <mltvrs/async/generator.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("iterating over a generator produces the correct results")
{
    CATCH_GIVEN("a generator that generates some series without yielding in the middle")
    {
        namespace ranges = std::ranges;
        namespace views  = ranges::views;

        const auto gen_coro =
            [](const ranges::input_range auto& input) -> mltvrs::async::generator<const int&>
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
    }
}
