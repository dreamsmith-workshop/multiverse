
template<typename Derived, typename T>
class mltvrs::async::detail::task_promise_base
{
    public:
        constexpr void return_value(std::convertible_to<T> auto&& retval)
        {
            static_cast<Derived*>(this)->do_return(std::forward<decltype(retval)>(retval));
        }
};

template<typename Derived>
class mltvrs::async::detail::task_promise_base<Derived, void>
{
    public:
        constexpr void return_void() const noexcept {}
};

template<typename T>
class mltvrs::async::task<T>::promise_type
    : public detail::task_promise_base<promise_type, value_type>
{
    public:
        [[nodiscard]] constexpr auto get_return_object() noexcept
        {
            return task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        constexpr auto initial_suspend() const { return std::suspend_never{}; }
        constexpr auto final_suspend() const noexcept { return std::suspend_always{}; }
        constexpr void unhandled_exception()
        {
            m_state = std::exception_ptr{std::current_exception()};
        }

    private:
        using value_storage_type = std::conditional_t<
            std::is_void_v<value_type>,
            std::type_identity<value_type>,
            std::conditional_t<std::is_reference_v<value_type>, value_type*, value_type>>;
        using state_type = std::variant<std::monostate, std::exception_ptr, value_storage_type>;

        friend class detail::task_promise_base<promise_type, value_type>;

        constexpr void do_return(std::convertible_to<value_type> auto&& retval)
            requires(!std::is_void_v<value_type>)
        {
            if constexpr(std::is_reference_v<value_type>) {
                m_state = std::addressof(retval);
            } else {
                m_state = std::forward<decltype(retval)>(retval);
            }
        }

        state_type m_state = {};
};

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
class mltvrs::async::sleep<Clock, WaitTraits, Executor>::promise_type
{
    public:
        constexpr void return_void() const noexcept {}
};

template<typename T>
constexpr mltvrs::async::task<T>::task(std::coroutine_handle<promise_type> coro) noexcept
    : m_coroutine{std::move(coro)}
{
}

template<typename T>
auto mltvrs::async::task<T>::operator=(task&& rhs) noexcept -> task&
{
    m_coroutine = std::exchange(rhs.m_coroutine, nullptr);
    return *this;
}

template<typename T>
mltvrs::async::task<T>::~task() noexcept
{
    if(m_coroutine) {
        m_coroutine.destroy();
    }
}

template<typename T>
constexpr void mltvrs::async::task<T>::await_suspend(std::coroutine_handle<> coroutine)
{
    m_coroutine = coroutine;
}

template<typename T>
[[nodiscard]] constexpr mltvrs::async::task<T>::operator bool() const noexcept
{
    return m_coroutine && !m_coroutine.done();
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(duration expiry)
    requires(detail::is_system_executor_v<executor_type> && !std::is_reference_v<Clock>)
    : sleep(expiry, executor_type{})
{
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(time_point expiry)
    requires(detail::is_system_executor_v<executor_type> && !std::is_reference_v<Clock>)
    : sleep(expiry, executor_type{})
{
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(duration expiry, executor_type exec)
    requires(!std::is_reference_v<Clock>)
    : m_timer{std::make_unique<timer_type>(exec, expiry)}
{
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(time_point expiry, executor_type exec)
    requires(!std::is_reference_v<Clock>)
    : m_timer{std::make_unique<timer_type>(exec, expiry)}
{
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(timer_type& preset_timer)
    requires(std::is_reference_v<Clock>)
    : m_timer{std::addressof(preset_timer)}
{
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(timer_type&& preset_timer)
    requires(!std::is_reference_v<Clock>)
    : m_timer{std::make_unique<timer_type>(std::move(preset_timer))}
{
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(timer_type& timer, duration expiry)
    requires(std::is_reference_v<Clock>)
    : m_timer{std::addressof(timer)}
{
    m_timer->expires_after(expiry);
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(timer_type&& timer, duration expiry)
    requires(!std::is_reference_v<Clock>)
    : m_timer{std::make_unique<timer_type>(std::move(timer))}
{
    m_timer->expires_after(expiry);
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(timer_type& timer, time_point expiry)
    requires(std::is_reference_v<Clock>)
    : m_timer{std::addressof(timer)}
{
    m_timer->expires_at(expiry);
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
mltvrs::async::sleep<Clock, WaitTraits, Executor>::sleep(timer_type&& timer, time_point expiry)
    requires(!std::is_reference_v<Clock>)
    : m_timer{std::make_unique<timer_type>(std::move(timer))}
{
    m_timer->expires_at(expiry);
}

template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
void mltvrs::async::sleep<Clock, WaitTraits, Executor>::await_suspend(
    std::coroutine_handle<> coroutine)
{
    auto& timer = *m_timer;
    timer.async_wait(
        [tmr = std::move(m_timer), coro = std::move(coroutine)](const auto& error)
        {
            if(!error) {
                coro();
            }
        });
}
