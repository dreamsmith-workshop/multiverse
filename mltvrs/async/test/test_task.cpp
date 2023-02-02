#include <mltvrs/async/task.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("synchronously waiting on a task produces the correct result")
{
    CATCH_GIVEN("a task that performs a calculation without yielding in the middle")
    {
        const auto task_coro = [](int arg) -> mltvrs::async::task<int> { co_return arg; };

        CATCH_WHEN("that task is executed once")
        {
            const auto data = GENERATE(take(5, random(-100, 100)));

            auto task = task_coro(data);

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
        CATCH_WHEN("that task is executed without the completion criteria being met")
        {
            CATCH_THEN("that task does not become ready")
            {
            }

            CATCH_AND_WHEN("that task is executed with the completion criteria met")
            {
                CATCH_THEN("that task becomes ready, and returns the correct value")
                {
                }
            }
        }
    }
}

CATCH_SCENARIO("asynchronously waiting on a task produces the correct result")
{
    CATCH_GIVEN("a task that performs a calculation without yielding in the middle")
    {
        CATCH_WHEN("that task is posted onto an executor")
        {
            CATCH_THEN("that task blocks the awaiter until it completes with the correct value")
            {
            }
        }
    }

    CATCH_GIVEN("a task that performs a calculation with internal yields")
    {
        CATCH_WHEN("that task is posted onto an executor without the completion criteria being met")
        {
            CATCH_THEN("that task blocks the awaiter")
            {
            }

            CATCH_AND_WHEN("that task has its completion criteria met")
            {
                CATCH_THEN("that task becomes ready, and returns the correct value")
                {
                }
            }
        }
    }
}
