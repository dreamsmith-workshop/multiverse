#pragma once

#include <mltvrs/async/coroutine.hpp>

namespace mltvrs::async {

    namespace detail {

        template<typename Derived, typename T>
        class task_promise_base;

        template<typename T>
        struct task_get_return
        {
                using type       = T&;
                using const_type = const T&;
        };

        template<>
        struct task_get_return<void>
        {
                using type       = void;
                using const_type = void;
        };

        template<typename T>
        using task_get_return_t = typename task_get_return<T>::type;
        template<typename T>
        using task_get_const_return_t = typename task_get_return<T>::const_type;

    } // namespace detail

    /**
     * @brief An exception indicating that the requested coroutine result is not available.
     */
    class no_result : public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };

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
            constexpr task(const task& rhs) = delete;
            //! @brief Move ownership of a coroutine into a new task.
            constexpr task(task&& rhs) noexcept;

            //! @brief Disable copy assignment.
            constexpr auto operator=(const task& rhs) -> task& = delete;
            //! @brief Move ownership of a coroutine via assignment.
            constexpr auto operator=(task&& rhs) noexcept -> task&;

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

            /**
             * @name Result
             *
             * @brief Check for, and obtain, the coroutine result.
             *
             * When `task::ready` returns `true`, the task has completed generating its result, and
             * `task::get` will return the generated result. When `task::ready` returns `false`,
             * then the task either has not run to completion, or has encountered an exception.
             * Callers can distinguish between these two states via `task::has_exception`.
             * `task::get_if` combines the presence check and result retrieval in one operation - it
             * returns a pointer to the result if it exists, and `nullptr` otherwise.
             *
             * If `task::get` is called when `task::has_exception` is `true`, the stored exception
             * will be thrown and propagated to the caller.
             *
             * @return Returns the requested result information.
             *
             * @throw async::no_result Attempting to retrieve a value the task yet to generated.
             */
            [[nodiscard]] constexpr bool ready() const noexcept;
            constexpr void               get() const
                requires(std::is_void_v<value_type>);
            [[nodiscard]] constexpr auto get() const -> detail::task_get_const_return_t<value_type>
                requires(!std::is_void_v<value_type>);
            [[nodiscard]] constexpr auto get() -> detail::task_get_return_t<value_type>
                requires(!std::is_void_v<value_type>);
            [[nodiscard]] constexpr auto get_if() const -> const value_type*;
            [[nodiscard]] constexpr auto get_if() -> value_type*;
            //! @}

            /**
             * @name Exceptions
             *
             * @brief Check for, and retrieve, exceptions thrown by the coroutine.
             *
             * When `task::has_exception` returns `true`, the task has stored an exception that has
             * escaped its coroutine's body, and `task::get_exception` will retrieve the stored
             * exception. If no exception is stored, `task::get_exception` returns a nulled value.
             *
             * @return Returns the requested exception information.
             *
             * @{
             */
            [[nodiscard]] constexpr bool has_exception() const noexcept;
            [[nodiscard]] auto           get_exception() const noexcept -> std::exception_ptr;
            //! @}

        private:
            std::coroutine_handle<promise_type> m_coroutine = nullptr;
    };

} // namespace mltvrs::async

#include <mltvrs/async/ipp/task.ipp>
