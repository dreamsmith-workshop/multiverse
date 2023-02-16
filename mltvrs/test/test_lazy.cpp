// clang-format off
#include <mltvrs/lazy.hpp>
// clang-format on

#include <stop_token>

#include <mltvrs/execution.hpp>

#include <mltvrs/async/concepts.hpp>

#include <catch2/catch_all.hpp>

CATCH_TEMPLATE_TEST_CASE(
    "waiting on a task produces the correct result",
    "[mltvrs][lazy][p1056]",
    std::size_t,
    std::vector<int>)
{
    CATCH_GIVEN("a task that performs a calculation")
    {
        const auto task_coro = [](TestType arg) -> mltvrs::lazy<TestType> { co_return arg; };

        CATCH_WHEN("that task is executed once")
        {
            const auto data = GENERATE(take(5, random(std::size_t{}, std::size_t{100})));

            const auto result = mltvrs::this_thread::sync_wait(task_coro(TestType(data)));

            CATCH_THEN("that task becomes ready, and returns the correct value")
            {
                CATCH_REQUIRE(result);
                CATCH_REQUIRE(std::get<TestType>(*result) == TestType(data));
            }
        }
    }

    CATCH_GIVEN("a task that does not produce a value")
    {
        const auto task_coro = []() -> mltvrs::lazy<void> { co_return; };

        CATCH_WHEN("that task is executed")
        {
            const auto result = mltvrs::this_thread::sync_wait(task_coro());

            CATCH_THEN("that task completes successfully")
            {
                CATCH_REQUIRE(result);
            }
        }
    }

    CATCH_GIVEN("a task that produces a reference result")
    {
        const auto data = TestType{};

        const auto task_coro = [](const TestType& arg) -> mltvrs::lazy<const TestType&>
        { co_return arg; };
        const auto wrapper_coro =
            [](mltvrs::async::awaitable auto coro) -> mltvrs::lazy<const TestType*>
        { co_return std::addressof(co_await coro); };

        CATCH_WHEN("that task is executed")
        {
            const auto result = mltvrs::this_thread::sync_wait(wrapper_coro(task_coro(data)));

            CATCH_THEN("that task becomes ready, and returns a reference to the correct object")
            {
                CATCH_REQUIRE(result);
                CATCH_REQUIRE(std::get<const TestType*>(*result) == std::addressof(data));
            }
        }
    }
}
