#include <stop_token>

#include <mltvrs/async/task.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("waiting on a task produces the correct result")
{
    CATCH_GIVEN("a task that performs a calculation without yielding in the middle")
    {
        const auto task_coro = [](int arg) -> mltvrs::async::task<int> { co_return arg; };

        CATCH_WHEN("that task is executed once")
        {
            const auto data = GENERATE(take(5, random(-100, 100)));

            auto task = task_coro(data);
            task();

            CATCH_THEN("that task becomes ready, and returns the correct value")
            {
                CATCH_REQUIRE(!task);
                CATCH_REQUIRE(task.ready());
                CATCH_REQUIRE(task.get() == data);
            }
        }
    }

    CATCH_GIVEN("a task that performs a calculation with internal yields")
    {
        auto       ssource   = std::stop_source{};
        const auto task_coro = [token = ssource.get_token()](int arg) -> mltvrs::async::task<int>
        {
            while(!token.stop_requested()) {
                co_await std::suspend_always{};
            }

            co_return arg;
        };

        CATCH_WHEN("that task is executed without the completion criteria being met")
        {
            const auto data = GENERATE(take(5, random(-100, 100)));

            auto task = task_coro(data);
            task();

            CATCH_THEN("that task does not become ready")
            {
                CATCH_REQUIRE(task);
                CATCH_REQUIRE(!task.ready());
            }

            CATCH_AND_WHEN("that task is executed with the completion criteria met")
            {
                ssource.request_stop();
                task();

                CATCH_THEN("that task becomes ready, and returns the correct value")
                {
                    CATCH_REQUIRE(!task);
                    CATCH_REQUIRE(task.ready());
                    CATCH_REQUIRE(task.get() == data);
                }
            }
        }
    }
}
