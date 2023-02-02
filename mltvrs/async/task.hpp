#pragma once

#include <mltvrs/async/coroutine.hpp>

namespace mltvrs::async {

    namespace detail {

        template<typename Derived, typename T>
        class task_promise_base;

    } // namespace detail

    /**
     * @brief A task coroutine with the given return type.
     *
     * @tparam T The type of result this task generates.
     */
    template<typename T = void>
    class task
    {
        public:
            using value_type = T; //!< The type of result this task generates.
            class promise_type;   //!< The coroutine promise type.

            //! @brief Construct an empty task with no associated coroutine.
            constexpr task() noexcept = default;
            //! @brief Construct a task representing the given coroutine.
            //! @param coro The coroutine this task represents.
            explicit constexpr task(std::coroutine_handle<promise_type> coro) noexcept;

            //! @brief Disable copy construction.
            task(const task& rhs) = delete;
            //! @brief Move ownership of a coroutine into a new task.
            task(task&& rhs) noexcept : m_coroutine{std::exchange(rhs.m_coroutine, nullptr)} {}

            //! @brief Disable copy assignment.
            auto operator=(const task& rhs) -> task& = delete;
            //! @brief Move ownership of a coroutine via assignment.
            auto operator=(task&& rhs) noexcept -> task&;

            ~task() noexcept; //!< Destroy a suspended coroutine.

            /**
             * @name Await
             *
             * @brief Suspend a task by storing its state.
             *
             * See [coroutines documentation](https://en.cppreference.com/w/cpp/language/coroutines)
             * for more information.
             *
             * @{
             */
            [[nodiscard]] constexpr bool await_ready() const noexcept { return false; }
            constexpr void               await_suspend(std::coroutine_handle<> coroutine);
            constexpr void               await_resume() {}
            //! @}

            //! @brief Resume execution.
            constexpr void operator()() { m_coroutine(); }
            //! @brief Check whether this coroutine can be resumed or not.
            //! @return Returns `true` if resumable, or `false` otherwise.
            [[nodiscard]] explicit constexpr operator bool() const noexcept;

            [[nodiscard]] constexpr bool ready() const noexcept;
            constexpr void               get() const
                requires(std::is_void_v<value_type>);
            [[nodiscard]] constexpr auto get() const -> const value_type&
                requires(!std::is_void_v<value_type>);
            [[nodiscard]] constexpr auto get() -> value_type&
                requires(!std::is_void_v<value_type>);

        private:
            std::coroutine_handle<promise_type> m_coroutine = nullptr;
    };

} // namespace mltvrs::async

#include <mltvrs/async/ipp/task.ipp>
