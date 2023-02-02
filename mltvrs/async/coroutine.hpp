#pragma once

#include <coroutine>
#include <ranges>
#include <variant>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/is_executor.hpp>

#include <mltvrs/chrono.hpp>

namespace mltvrs::async {

    template<typename T>
    concept executor =
        boost::asio::is_executor<T>::value || std::same_as<T, boost::asio::any_io_executor>;

    template<typename T>
    concept execution_context = std::derived_from<T, boost::asio::execution_context>
                             && executor<typename T::executor_type> &&
                                // clang-format off
        requires(T ctx) {
            { ctx.get_executor() } -> std::same_as<typename T::executor_type>;
        }; // clang-format on

    namespace detail {

        template<typename T>
        struct is_system_executor : public std::false_type
        {
        };

        template<typename... T>
        struct is_system_executor<boost::asio::basic_system_executor<T...>> : public std::true_type
        {
        };

        template<typename T>
        inline constexpr bool is_system_executor_v = is_system_executor<T>::value;

        template<typename Derived, typename T>
        class task_promise_base;

    } // namespace detail

    template<typename T = void>
    class task
    {
        public:
            using value_type = T;

            class promise_type;

            constexpr task() noexcept = default;
            explicit constexpr task(std::coroutine_handle<promise_type> coro) noexcept;

            task(const task& rhs) = delete;
            task(task&& rhs) noexcept : m_coroutine{std::exchange(rhs.m_coroutine, nullptr)} {}

            auto operator=(const task& rhs) -> task& = delete;
            auto operator=(task&& rhs) noexcept -> task&;

            ~task() noexcept;

            [[nodiscard]] constexpr bool await_ready() const noexcept { return false; }
            constexpr void               await_suspend(std::coroutine_handle<> coroutine);
            constexpr void               await_resume() {}

            constexpr void operator()() { m_coroutine(); }
            [[nodiscard]] explicit constexpr operator bool() const noexcept;

        private:
            std::coroutine_handle<promise_type> m_coroutine = nullptr;
    };

    template<
        chrono::clock Clock = std::chrono::system_clock,
        typename WaitTraits = typename boost::asio::basic_waitable_timer<Clock>::traits_type,
        executor Executor   = boost::asio::system_executor>
    class sleep
    {
        public:
            using clock      = Clock;
            using time_point = typename clock::time_point;
            using duration   = typename clock::duration;
            using rep        = typename clock::rep;
            using period     = typename clock::period;

            using clock_type    = clock;
            using traits_type   = WaitTraits;
            using executor_type = Executor;

            using timer_type = boost::asio::basic_waitable_timer<clock, traits_type, executor_type>;

            class promise_type;

            explicit sleep(duration expiry)
                requires(detail::is_system_executor_v<executor_type>);
            explicit sleep(time_point expiry)
                requires(detail::is_system_executor_v<executor_type>);
            sleep(duration expiry, executor_type exec);
            sleep(time_point expiry, executor_type exec);

            bool await_ready() const noexcept { return false; }
            void await_suspend(std::coroutine_handle<> coroutine);
            void await_resume() {}

        private:
            std::unique_ptr<timer_type> m_timer;
    };

    template<typename... D>
    sleep(std::chrono::duration<D...>) -> sleep<>;

    template<typename... D, executor Executor>
    sleep(std::chrono::duration<D...>, const Executor&) -> sleep<
        std::chrono::system_clock,
        typename boost::asio::basic_waitable_timer<std::chrono::system_clock>::traits_type,
        Executor>;

    template<typename Clock, typename... T>
    sleep(std::chrono::time_point<Clock, T...>) -> sleep<Clock>;

    template<typename Clock, typename... T, executor Executor>
    sleep(std::chrono::time_point<Clock, T...>, const Executor&)
        -> sleep<Clock, typename boost::asio::basic_waitable_timer<Clock>::traits_type, Executor>;

} // namespace mltvrs::async

#include <mltvrs/async/ipp/coroutine.ipp>
