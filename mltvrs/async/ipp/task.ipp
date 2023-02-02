
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

        [[nodiscard]] constexpr bool ready() const noexcept
        {
            return std::holds_alternative<value_storage_type>(m_state);
        }

        constexpr void get() const
        {
            if(!get_if()) {
                throw no_exception{"requesting the stored exception when this coroutine has none"};
            }
        }

        [[nodiscard]] constexpr auto& get() const
            requires(!std::is_void_v<value_type>)
        {
            return std::visit(
                [](const auto& state) -> const value_type&
                {
                    using active_state = std::remove_cvref_t<decltype(state)>;
                    if constexpr(std::same_as<active_state, std::monostate>) {
                        throw no_result{
                            "requesting the task result when it has yet to generate one"};
                    } else if constexpr(std::same_as<active_state, std::exception_ptr>) {
                        std::rethrow_exception(state);
                    } else {
                        return state;
                    }
                },
                m_state);
        }

        [[nodiscard]] constexpr auto& get()
            requires(!std::is_void_v<value_type>)
        {
            return std::visit(
                [](auto& state) -> value_type&
                {
                    using active_state = std::remove_cvref_t<decltype(state)>;
                    if constexpr(std::same_as<active_state, std::monostate>) {
                        throw no_result{
                            "requesting the task result when it has yet to generate one"};
                    } else if constexpr(std::same_as<active_state, std::exception_ptr>) {
                        std::rethrow_exception(state);
                    } else {
                        return state;
                    }
                },
                m_state);
        }

        [[nodiscard]] constexpr auto get_if() const -> const std::remove_reference_t<value_type>*
        {
            if(const auto* const retval = std::get_if<value_storage_type>(m_state)) {
                if constexpr(std::is_void_v<value_type>) {
                    return this;
                } else if constexpr(std::is_reference_v<value_type>) {
                    return *retval;
                } else {
                    return retval;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr auto get_if() -> std::remove_reference_t<value_type>*
        {
            if(auto* const retval = std::get_if<value_storage_type>(m_state)) {
                if constexpr(std::is_void_v<value_type>) {
                    return this;
                } else if constexpr(std::is_reference_v<value_type>) {
                    return *retval;
                } else {
                    return retval;
                }
            }

            return nullptr;
        }

        [[nodiscard]] constexpr bool has_exception() const noexcept
        {
            return std::holds_alternative<std::exception_ptr>(m_state);
        }

        [[nodiscard]] auto get_exception() const noexcept -> std::exception_ptr
        {
            if(const auto* const retval = std::get_if<std::exception_ptr>(m_state)) {
                return *retval;
            }

            return nullptr;
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

template<typename T>
[[nodiscard]] constexpr bool mltvrs::async::task<T>::ready() const noexcept
{
    return m_coroutine.promise().ready();
}

template<typename T>
constexpr void mltvrs::async::task<T>::get() const
    requires(std::is_void_v<value_type>)
{
    m_coroutine.promise().get();
}

template<typename T>
[[nodiscard]] constexpr auto mltvrs::async::task<T>::get() const -> const value_type&
    requires(!std::is_void_v<value_type>)
{
    return m_coroutine.promise().get();
}

template<typename T>
[[nodiscard]] constexpr auto mltvrs::async::task<T>::get() -> value_type&
    requires(!std::is_void_v<value_type>)
{
    return m_coroutine.promise().get();
}

template<typename T>
[[nodiscard]] constexpr auto mltvrs::async::task<T>::get_if() const -> const value_type*
{
    return m_coroutine.promise().get_if();
}

template<typename T>
[[nodiscard]] constexpr auto mltvrs::async::task<T>::get_if() -> value_type*
{
    return m_coroutine.promise().get_if();
}

template<typename T>
[[nodiscard]] constexpr bool mltvrs::async::task<T>::has_exception() const noexcept
{
    return m_coroutine.promise().has_exception();
}

template<typename T>
[[nodiscard]] auto mltvrs::async::task<T>::get_exception() const noexcept -> std::exception_ptr
{
    return m_coroutine.promise().get_exception();
}
