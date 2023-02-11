
namespace mltvrs::async::detail {

    [[nodiscard]] constexpr auto coro_handle_of(coroutine auto&& coro) noexcept
    {
        return coroutine_traits<std::remove_cvref_t<decltype(coro)>>::handle_of(
            std::forward<decltype(coro)>(coro));
    }

} // namespace mltvrs::async::detail

void mltvrs::async::execute(executable auto&& coro)
{
    const auto handle = detail::coro_handle_of(std::forward<decltype(coro)>(coro));
    do {
        handle.resume();
    } while(!handle.done());
}

void mltvrs::async::execute_once(executable auto&& coro)
{
    detail::coro_handle_of(std::forward<decltype(coro)>(coro)).resume();
}

template<typename... D>
void mltvrs::async::execute_for(executable auto&& coro, std::chrono::duration<D...> timeout)
{
    const auto end = std::chrono::steady_clock::now() + timeout;
    do {
        execute_once(coro);
    } while(std::chrono::steady_clock::now() < end);
}

template<typename... P>
void mltvrs::async::execute_until(executable auto&& coro, std::chrono::time_point<P...> timeout)
{
    using clock = typename std::chrono::time_point<P...>::clock;
    do {
        execute_once(coro);
    } while(clock::now() < timeout);
}
