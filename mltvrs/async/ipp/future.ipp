
namespace mltvrs::async::detail {

    struct value_retrieved
    {
    };

    template<executor Executor>
    struct post_to
    {
        public:
            post_to(const Executor& exec) noexcept : m_executor{exec} {}

            constexpr bool await_ready() const noexcept { return false; }

            void await_suspend(std::coroutine_handle<> awaiter)
            {
                boost::asio::post(m_executor, [awaiter] { awaiter(); });
            }

            constexpr void await_resume() const noexcept {}

        private:
            Executor m_executor;
    };

} // namespace mltvrs::async::detail

template<typename T>
class mltvrs::async::future<T>::promise_type
{
    private:
        using value_storage_type = std::conditional_t<
            std::is_void_v<value_type>,
            std::type_identity<void>,
            std::conditional_t<
                std::is_reference_v<value_type>,
                std::add_pointer_t<value_type>,
                value_type>>;

    public:
        auto get_return_object() noexcept
        {
            return future{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        void get()
            requires(std::is_void_v<value_type>)
        {
            wait();
            std::visit(
                [](const auto& result)
                {
                    using result_type = std::remove_cvref_t<decltype(result)>;
                    if constexpr(std::same_as<result_type, std::monostate>) {
                        throw std::future_error{std::future_errc::broken_promise};
                    } else if constexpr(std::same_as<result_type, detail::value_retrieved>) {
                        throw std::future_error{std::future_errc::future_already_retrieved};
                    } else {
                        static_assert(std::same_as<result_type, value_storage_type>);
                    }
                },
                m_result);

            m_result = detail::value_retrieved{};
        }

        [[nodiscard]] auto get() -> value_type
            requires(!std::is_void_v<value_type>)
        {
            wait();
            auto ret = std::visit(
                [](auto& result) -> value_storage_type
                {
                    using result_type = std::remove_cvref_t<decltype(result)>;
                    if constexpr(std::same_as<result_type, std::monostate>) {
                        throw std::future_error{std::future_errc::broken_promise};
                    } else if constexpr(std::same_as<result_type, detail::value_retrieved>) {
                        throw std::future_error{std::future_errc::future_already_retrieved};
                    } else {
                        static_assert(std::same_as<result_type, value_storage_type>);
                        return std::move(result);
                    }
                },
                m_result);
            m_result = detail::value_retrieved{};

            if constexpr(std::is_reference_v<value_type>) {
                return *ret;
            } else {
                return ret;
            }
        }

        void wait() const
        {
            if(!m_semaphore.try_acquire()) {
                m_semaphore.acquire();
            }
        }

        template<typename... D>
        [[nodiscard]] auto wait_for(const std::chrono::duration<D...> timeout) const
            -> std::future_status
        {
            return !m_semaphore.try_acquire()           ? std::future_status::ready
                 : m_semaphore.try_acquire_for(timeout) ? std::future_status::ready
                                                        : std::future_status::timeout;
        }

        template<typename... P>
        [[nodiscard]] auto wait_until(const std::chrono::time_point<P...> timeout) const
            -> std::future_status
        {
            return !m_semaphore.try_acquire()             ? std::future_status::ready
                 : m_semaphore.try_acquire_until(timeout) ? std::future_status::ready
                                                          : std::future_status::timeout;
        }

        [[nodiscard]] auto then(
            std::coroutine_handle<promise_type> self,
            std::invocable<value_type> auto&&   continuation)
            -> future<std::invoke_result_t<decltype(continuation), value_type>>
        {
            constexpr auto do_continue = [](std::coroutine_handle<promise_type> slf,
                                            std::invocable<future> auto&&       cont,
                                            const executor auto&                exec)
                -> future<std::invoke_result_t<decltype(cont), value_type>>
            {
                co_await detail::post_to{exec};

                if constexpr(std::is_void_v<value_type>) {
                    co_return std::invoke(std::forward<decltype(cont)>(cont));
                } else {
                    co_return std::invoke(
                        std::forward<decltype(cont)>(cont),
                        std::get<value_storage_type>(slf.promise().m_result));
                }
            };

            return do_continue(
                self,
                std::forward<decltype(continuation)>(continuation),
                get_associated_executor(continuation));
        }

    private:
        using result_state =
            std::variant<std::monostate, detail::value_retrieved, value_storage_type>;

        std::coroutine_handle<>       m_continuation{nullptr};
        result_state                  m_result{};
        mutable std::binary_semaphore m_semaphore{0};
};

template<typename T>
mltvrs::async::future<T>::future(std::coroutine_handle<promise_type> coro) noexcept
    : m_coroutine{coro}
{
}

template<typename T>
mltvrs::async::future<T>::future(future&& other) noexcept
    : m_coroutine{std::exchange(other.m_coroutine, nullptr)}
{
}

template<typename T>
auto mltvrs::async::future<T>::operator=(future&& other) noexcept -> future&
{
    if(this == std::addressof(other)) {
        return *this;
    }
    if(m_coroutine) {
        wait();
        m_coroutine.destroy();
    }

    m_coroutine = std::exchange(other.m_coroutine, nullptr);

    return *this;
}

template<typename T>
mltvrs::async::future<T>::~future() noexcept
{
    if(m_coroutine) {
        wait();
        m_coroutine.destroy();
    }
}

template<typename T>
void mltvrs::async::future<T>::get() const
    requires(std::is_void_v<value_type>)
{
    m_coroutine.promise().get();
}

template<typename T>
[[nodiscard]] auto mltvrs::async::future<T>::get() const -> value_type
    requires(!std::is_void_v<value_type>)
{
    return m_coroutine.promise().get();
}

template<typename T>
void mltvrs::async::future<T>::wait() const
{
    m_coroutine.promise().wait();
}

template<typename T>
template<typename... D>
[[nodiscard]] auto
mltvrs::async::future<T>::wait_for(const std::chrono::duration<D...> timeout) const
    -> std::future_status
{
    return m_coroutine.promise().wait_for(timeout);
}

template<typename T>
template<typename... P>
[[nodiscard]] auto
mltvrs::async::future<T>::wait_until(const std::chrono::time_point<P...> timeout) const
    -> std::future_status
{
    return m_coroutine.promise().wait_until(timeout);
}

template<typename T>
[[nodiscard]] auto mltvrs::async::future<T>::then(std::invocable<value_type> auto&& continuation)
    -> future<std::invoke_result_t<decltype(continuation), value_type>>
{
    const auto  coro = m_coroutine;
    const auto& exec = get_associated_executor(continuation);

    return coro.promise().then(
        std::exchange(m_coroutine, nullptr),
        std::forward<decltype(continuation)>(continuation),
        exec);
}

template<typename T>
[[nodiscard]] auto mltvrs::async::future<T>::then(
    std::invocable<value_type> auto&& continuation,
    const executor auto& exec) -> future<std::invoke_result_t<decltype(continuation), value_type>>
{
    const auto coro = m_coroutine;

    return coro.promise().then(
        std::exchange(m_coroutine, nullptr),
        std::forward<decltype(continuation)>(continuation),
        exec);
}
