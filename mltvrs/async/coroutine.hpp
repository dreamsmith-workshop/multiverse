#pragma once

#include <coroutine>
#include <ranges>
#include <variant>

#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/is_executor.hpp>

#include <mltvrs/chrono.hpp>

#include <mltvrs/async/concepts.hpp>

namespace mltvrs::async {

    namespace detail {

        inline constexpr auto noop = [](auto&&...) {};
        using noop_t               = std::remove_cvref_t<decltype(noop)>;

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

    } // namespace detail

    /**
     * @brief A coroutine that suspends execution for a duration or until a certain time.
     *
     * Suspend the caller, and schedule it for resumption on the given executor once the given time
     * has elapsed.
     *
     * @tparam Clock      The clock to time the suspension on.
     * @tparam WaitTraits The timer wait traits.
     * @tparam Executor   The executor to resume execution on once the requisite time has elapsed.
     */
    template<
        typename Clock      = std::chrono::system_clock,
        typename WaitTraits = typename boost::asio::basic_waitable_timer<Clock>::traits_type,
        executor Executor   = boost::asio::system_executor>
        requires(chrono::clock<std::remove_cvref_t<Clock>>)
    class sleep
    {
        public:
            using clock      = std::remove_cvref_t<Clock>; //!< The clock to time the suspension on.
            using time_point = typename clock::time_point; //!< The point-in-time type.
            using duration   = typename clock::duration;   //!< The duration type.
            using rep        = typename clock::rep;        //!< The duration numeric type.
            using period     = typename clock::period;     //!< The to-second duration ratio type.

            using clock_type    = clock;                   //!< Asio-like clock type alias.
            using traits_type   = WaitTraits;              //!< Asio timer wait traits.
            using executor_type = Executor;                //!< The associated executor type.

            //! The sleep timer type.
            using timer_type = boost::asio::basic_waitable_timer<clock, traits_type, executor_type>;

            class promise_type; //!< The coroutine promise type.

            /**
             * @name Construct with New Timer
             *
             * @brief Suspend coroutine execution for the given duration or until the given time.
             *
             * When the associated executor type is a specialization of
             * `boost::asio::basic_system_executor`, the explicit executor argument is optional.
             *
             * @param expiry The timer expiration duration or point-in-time.
             * @param exec   The execution context to resume execution on after waking up.
             *
             * @{
             */
            explicit sleep(duration expiry)
                requires(detail::is_system_executor_v<executor_type> && !std::is_reference_v<Clock>)
            ;
            explicit sleep(time_point expiry)
                requires(detail::is_system_executor_v<executor_type> && !std::is_reference_v<Clock>)
            ;
            sleep(duration expiry, executor_type exec)
                requires(!std::is_reference_v<Clock>);
            sleep(time_point expiry, executor_type exec)
                requires(!std::is_reference_v<Clock>);
            //! @}

            /**
             * @name Construct with Existing Timer
             *
             * @brief Suspend coroutine execution until the given timer expires.
             *
             * @param preset_timer A timer that is already set for future expiration.
             * @param timer        A timer to set with the given expiration time.
             * @param expiry       The expiration time to set the given timer with.
             *
             * @{
             */
            explicit sleep(timer_type& preset_timer)
                requires(std::is_reference_v<Clock>);
            explicit sleep(timer_type&& preset_timer)
                requires(!std::is_reference_v<Clock>);
            sleep(timer_type& timer, duration expiry)
                requires(std::is_reference_v<Clock>);
            sleep(timer_type&& timer, duration expiry)
                requires(!std::is_reference_v<Clock>);
            sleep(timer_type& timer, time_point expiry)
                requires(std::is_reference_v<Clock>);
            sleep(timer_type&& timer, time_point expiry)
                requires(!std::is_reference_v<Clock>);
            //! @}

            /**
             * @name Await
             *
             * @brief Schedule execution resumption on the associated executor on timer expiration.
             *
             * See [coroutines documentation](https://en.cppreference.com/w/cpp/language/coroutines)
             * for more information.
             *
             * @{
             */
            bool await_ready() const noexcept { return false; }
            void await_suspend(std::coroutine_handle<> coroutine);
            void await_resume() {}
            //! @}

        private:
            using timer_storage_type = std::conditional_t<
                std::is_reference_v<Clock>,
                std::unique_ptr<timer_type, detail::noop_t>,
                std::unique_ptr<timer_type>>;

            timer_storage_type m_timer;
    };

    //! Assume resumption on the system executor after the given duration.
    template<typename... D>
    sleep(std::chrono::duration<D...>) -> sleep<>;
    //! Resume on the given executor after the given duration.
    template<typename... D, executor Executor>
    sleep(std::chrono::duration<D...>, const Executor&) -> sleep<
        std::chrono::system_clock,
        typename boost::asio::basic_waitable_timer<std::chrono::system_clock>::traits_type,
        Executor>;
    //! Assume resumption on the system executor when the given point-in-time is reached.
    template<typename Clock, typename... T>
    sleep(std::chrono::time_point<Clock, T...>) -> sleep<Clock>;
    //! Resume on the given executor when the given point-in-time is reached.
    template<typename Clock, typename... T, executor Executor>
    sleep(std::chrono::time_point<Clock, T...>, const Executor&)
        -> sleep<Clock, typename boost::asio::basic_waitable_timer<Clock>::traits_type, Executor>;
    //! Use the given existing timer instead of creating a new one.
    template<typename Clock, typename... T, typename... A>
    sleep(boost::asio::basic_waitable_timer<Clock, T...>&, A&&...) -> sleep<
        Clock&,
        typename boost::asio::basic_waitable_timer<Clock, T...>::traits_type,
        typename boost::asio::basic_waitable_timer<Clock, T...>::executor_type>;

} // namespace mltvrs::async

#include <mltvrs/async/ipp/coroutine.ipp>
