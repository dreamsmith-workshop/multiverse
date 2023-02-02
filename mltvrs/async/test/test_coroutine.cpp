#include <thread>

#include <boost/asio/thread_pool.hpp>

#include <mltvrs/async/coroutine.hpp>

#include <catch2/catch_all.hpp>

CATCH_SCENARIO("putting a coroutine to sleep causes it to suspend for the desired amount of time")
{
    CATCH_GIVEN("a coroutine that appends timestamps to a sequence at regular intervals")
    {
        using namespace std::chrono_literals;

        using clock      = std::chrono::system_clock;
        using duration   = std::chrono::duration<int, std::ratio<1, 10>>;
        using time_point = std::chrono::system_clock::time_point;

        constexpr auto itrs    = 10;
        constexpr auto itr_dur = 200ms;

        auto       pool       = boost::asio::thread_pool{1};
        auto       mtx        = std::mutex{};
        auto       timestamps = std::vector<time_point>{};
        const auto coro_def   = [&](time_point start) -> mltvrs::async::task<>
        {
            for(auto i = 0; i < itrs; ++i) {
                co_await mltvrs::async::sleep(start + (itr_dur * i), pool.get_executor());

                const auto guard = std::lock_guard{mtx};
                timestamps.push_back(clock::now());
            }
        };

        CATCH_WHEN("that coroutine is allowed to run to completion")
        {
            const auto start = clock::now();
            auto       coro  = coro_def(start);
            std::this_thread::sleep_for((itrs + 1) * itr_dur);

            const auto expected_elapsed = [&]
            {
                auto ret = std::vector<double>{};
                for(auto i = 0; i < itrs; ++i) {
                    ret.push_back(std::chrono::duration_cast<duration>(itr_dur * i).count());
                }

                return ret;
            }();

            CATCH_THEN("that coroutine will have recorded the expected timestamps")
            {
                const auto guard = std::lock_guard{mtx};
                const auto record_elapsed_view =
                    timestamps
                    | std::ranges::views::transform(
                        [&](const auto& time)
                        { return std::chrono::duration_cast<duration>(time - start).count(); });
                const auto record_elapsed = std::vector<double>{
                    std::ranges::cbegin(record_elapsed_view),
                    std::ranges::cend(record_elapsed_view)};

                CATCH_REQUIRE_THAT(record_elapsed, Catch::Matchers::Approx(expected_elapsed));
            }
        }
    }
}
