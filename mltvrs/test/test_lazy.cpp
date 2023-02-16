// clang-format off
#include <mltvrs/lazy.hpp>
// clang-format on

#include <stop_token>

#include <mltvrs/execution.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("waiting on a task produces the correct result")
{
    CATCH_GIVEN("a task that performs a calculation")
    {
        const auto task_coro = [](int arg) -> mltvrs::lazy<int> { co_return arg; };

        CATCH_WHEN("that task is executed once")
        {
            const auto data = GENERATE(take(5, random(-100, 100)));

            const auto result = mltvrs::this_thread::sync_wait(task_coro(data));

            CATCH_THEN("that task becomes ready, and returns the correct value")
            {
                CATCH_REQUIRE(result);
                CATCH_REQUIRE(std::get<int>(*result) == data);
            }
        }
    }
}
