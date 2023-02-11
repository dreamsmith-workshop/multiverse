
namespace mltvrs::async::detail {

    [[nodiscard]] constexpr auto coro_handle_of(coroutine auto&& coro) noexcept
    {
        return coroutine_traits<std::remove_cvref_t<decltype(coro)>>::handle_of(
            std::forward<decltype(coro)>(coro));
    }

    template<typename... P>
    void execute_until(const coro_handle auto& coro, std::chrono::time_point<P...> timeout)
    {
        using clock = typename std::chrono::time_point<P...>::clock;
        while(!coro.done() && (clock::now() < timeout)) {
            coro.resume();
        }
    }

} // namespace mltvrs::async::detail

void mltvrs::async::execute(executable auto&& coro)
{
    const auto handle = detail::coro_handle_of(std::forward<decltype(coro)>(coro));
    while(!handle.done()) {
        handle.resume();
    }
}

void mltvrs::async::execute_once(executable auto&& coro)
{
    detail::coro_handle_of(std::forward<decltype(coro)>(coro)).resume();
}

template<typename... D>
void mltvrs::async::execute_for(executable auto&& coro, std::chrono::duration<D...> timeout)
{
    detail::execute_until(
        detail::coro_handle_of(std::forward<decltype(coro)>(coro)),
        std::chrono::steady_clock::now() + timeout);
}

template<typename... P>
void mltvrs::async::execute_until(executable auto&& coro, std::chrono::time_point<P...> timeout)
{
    detail::execute_until(detail::coro_handle_of(std::forward<decltype(coro)>(coro)), timeout);
}
