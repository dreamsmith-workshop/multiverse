#pragma once

#include <mltvrs/async/concepts.hpp>
#include <mltvrs/async/type_traits.hpp>

namespace mltvrs::this_thread {

    void execute(async::executable auto&& coro);
    void execute_once(async::executable auto&& coro);
    template<typename... D>
    void execute_for(async::executable auto&& coro, std::chrono::duration<D...> timeout);
    template<typename... P>
    void execute_until(async::executable auto&& coro, std::chrono::time_point<P...> timeout);

} // namespace mltvrs::this_thread

#include <mltvrs/async/ipp/this_thread.ipp>
