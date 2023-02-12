
namespace mltvrs::this_thread::detail {

    [[nodiscard]] constexpr auto coro_handle_of(async::coroutine auto&& coro) noexcept
    {
        return async::coroutine_traits<std::remove_cvref_t<decltype(coro)>>::handle_of(
            std::forward<decltype(coro)>(coro));
    }

    template<typename... P>
    void execute_until(
        const async::detail::coro_handle auto& coro,
        std::chrono::time_point<P...>          timeout)
    {
        using clock = typename std::chrono::time_point<P...>::clock;
        while(!coro.done() && (clock::now() < timeout)) {
            coro.resume();
        }
    }

} // namespace mltvrs::this_thread::detail

void mltvrs::this_thread::execute(async::executable auto&& coro)
{
    const auto handle = detail::coro_handle_of(std::forward<decltype(coro)>(coro));
    while(!handle.done()) {
        handle.resume();
    }
}

void mltvrs::this_thread::execute_once(async::executable auto&& coro)
{
    detail::coro_handle_of(std::forward<decltype(coro)>(coro)).resume();
}

template<typename... D>
void mltvrs::this_thread::execute_for(
    async::executable auto&&    coro,
    std::chrono::duration<D...> timeout)
{
    detail::execute_until(
        detail::coro_handle_of(std::forward<decltype(coro)>(coro)),
        std::chrono::steady_clock::now() + timeout);
}

template<typename... P>
void mltvrs::this_thread::execute_until(
    async::executable auto&&      coro,
    std::chrono::time_point<P...> timeout)
{
    detail::execute_until(detail::coro_handle_of(std::forward<decltype(coro)>(coro)), timeout);
}
