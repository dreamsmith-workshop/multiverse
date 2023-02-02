
template<typename Clock, typename WaitTraits, mltvrs::async::executor Executor>
    requires(mltvrs::chrono::clock<std::remove_cvref_t<Clock>>)
class mltvrs::async::sleep<Clock, WaitTraits, Executor>::promise_type
{
    public:
        constexpr void return_void() const noexcept {}
};

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
