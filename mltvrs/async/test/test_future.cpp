#include <mltvrs/async/execute.hpp>
#include <mltvrs/async/future.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("a coroutine returning a future allows the caller to obtain the result")
{
    using namespace std::chrono_literals;

    CATCH_GIVEN("a couroutine that returns a result without yielding in the middle")
    {
        constexpr auto coro = [](int arg) -> mltvrs::async::future<int> { co_return arg; };

        CATCH_WHEN("that coroutine is executed")
        {
            const auto data = GENERATE(take(5, random(-100, 100)));

            auto future = coro(data);
            mltvrs::async::execute(future);

            CATCH_THEN("the associated future becomes ready, and returns the correct value")
            {
                CATCH_REQUIRE(future.wait_for(0s) == std::future_status::ready);
                CATCH_REQUIRE(future.get() == data);
                CATCH_REQUIRE_THROWS_AS(future.get(), std::future_error);
            }
        }
    }

    CATCH_GIVEN("a coroutine that returns a result with internal yields")
    {
        auto       finish = std::atomic<bool>{false};
        const auto coro   = [&](int arg) -> mltvrs::async::future<int>
        {
            while(!finish) {
                co_await std::suspend_always{};
            }

            co_return arg;
        };

        CATCH_WHEN("that coroutine is executed without the completion criteria being met")
        {
            const auto data = GENERATE(take(5, random(-100, 100)));

            auto future = coro(data);
            mltvrs::async::execute_for(future, 10ms);

            CATCH_THEN("the associated future does not become ready")
            {
                CATCH_REQUIRE(future.wait_for(1s) == std::future_status::timeout);
            }

            CATCH_WHEN("that coroutine is executed with the completion criteria met")
            {
                finish = true;
                mltvrs::async::execute_for(future, 10ms);

                CATCH_THEN("the associated future becomes ready, and returns the correct value")
                {
                    CATCH_REQUIRE(future.wait_for(0s) == std::future_status::ready);
                    CATCH_REQUIRE(future.get() == data);
                    CATCH_REQUIRE_THROWS_AS(future.get(), std::future_error);
                }
            }
        }
    }
}
