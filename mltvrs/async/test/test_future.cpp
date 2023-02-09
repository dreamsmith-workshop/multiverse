#include <mltvrs/async/future.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("a coroutine returning a future allows the caller to obtain the result")
{
    CATCH_GIVEN("a couroutine that returns a result without yielding in the middle")
    {
        CATCH_WHEN("that coroutine is executed")
        {
            CATCH_THEN("the associated future becomes ready, and returns the correct value")
            {
            }
        }
    }

    CATCH_GIVEN("a coroutine that returns a result with internal yields")
    {
        CATCH_WHEN("that coroutine is executed without the completion criteria being met")
        {
            CATCH_THEN("the associated future does not become ready")
            {
            }

            CATCH_AND_WHEN("that coroutine is executed with the completion criteria met")
            {
                CATCH_THEN("the associated future becomes ready, and returns the correct value")
                {
                }
            }
        }
    }
}
