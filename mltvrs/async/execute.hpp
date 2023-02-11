#pragma once

#include <mltvrs/async/concepts.hpp>
#include <mltvrs/async/type_traits.hpp>

namespace mltvrs::async {

    void execute(executable auto&& coro);
    void execute_once(executable auto&& coro);
    template<typename... D>
    void execute_for(executable auto&& coro, std::chrono::duration<D...> timeout);
    template<typename... P>
    void execute_until(executable auto&& coro, std::chrono::time_point<P...> timeout);

} // namespace mltvrs::async

#include <mltvrs/async/ipp/execute.ipp>
