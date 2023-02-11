#pragma once

#include <chrono>
#include <concepts>
#include <coroutine>
#include <future>
#include <optional>
#include <type_traits>
#include <variant>

#include <gsl/util>

#include <boost/asio/associated_executor.hpp>
#include <boost/asio/post.hpp>

#include <mltvrs/async/concepts.hpp>
#include <mltvrs/async/type_traits.hpp>

namespace mltvrs::async {

    template<typename T>
    class future;

    namespace detail {

        template<typename T>
        class continuation_base;

        template<typename T>
        [[nodiscard]] auto handle_of(const future<T>& fut) noexcept;

    } // namespace detail

    using boost::asio::associated_executor;
    using boost::asio::associated_executor_t;
    using boost::asio::get_associated_executor;

    template<typename T>
    class future
    {
        public:
            using value_type = T;
            class promise_type;

            future(const future& other) = delete;
            future(future&& other) noexcept;

            auto operator=(const future& other) -> future& = delete;
            auto operator=(future&& other) noexcept -> future&;

            ~future() noexcept;

            void get() const
                requires(std::is_void_v<value_type>);
            [[nodiscard]] auto get() const -> value_type
                requires(!std::is_void_v<value_type>);

            void wait() const;
            template<typename... D>
            [[nodiscard]] auto wait_for(const std::chrono::duration<D...> timeout) const
                -> std::future_status;
            template<typename... P>
            [[nodiscard]] auto wait_until(const std::chrono::time_point<P...> timeout) const
                -> std::future_status;

            [[nodiscard]] auto then(std::invocable<value_type> auto&& continuation)
                -> future<std::invoke_result_t<decltype(continuation), value_type>>;
            [[nodiscard]] auto
            then(std::invocable<value_type> auto&& continuation, const executor auto& exec)
                -> future<std::invoke_result_t<decltype(continuation), value_type>>;

            [[nodiscard]] friend auto
            operator|(future&& fut, std::invocable<value_type> auto&& continuation)
            {
                return fut.then(std::forward<decltype(continuation)>(continuation));
            }

        private:
            friend auto detail::handle_of(const future& fut) noexcept;

            explicit future(std::coroutine_handle<promise_type> coro) noexcept;

            std::coroutine_handle<promise_type> m_coroutine;
    };

} // namespace mltvrs::async

template<typename T>
struct mltvrs::async::coroutine_traits<mltvrs::async::future<T>>
    : public std::coroutine_traits<future<T>>
{
    public:
        using coroutine_type = future<T>;
        using promise_type   = promise_type_t<coroutine_type>;

        [[nodiscard]] static auto handle_of(const coroutine_type& coro) noexcept
            -> std::coroutine_handle<promise_type>;
        [[nodiscard]] static auto handle_of(coroutine_type& coro) noexcept
            -> std::coroutine_handle<promise_type>;
};

#include <mltvrs/async/ipp/future.ipp>
