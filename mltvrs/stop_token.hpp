#pragma once

#include <stop_token>

#if __has_include(<stdexec/stop_token.hpp>)
#include <stdexec/stop_token.hpp>
#define MLTVRS_EXECUTION_STOP_TOKEN_IMPL_STDEXEC true
#else
#define MLTVRS_EXECUTION_STOP_TOKEN_IMPL_STDEXEC false
#endif

namespace mltvrs {

#if(MLTVRS_EXECUTION_STOP_TOKEN_IMPL_STDEXEC)
    using stdexec::in_place_stop_callback;
    using stdexec::in_place_stop_source;
    using stdexec::in_place_stop_token;
    using stdexec::never_stop_token;
    using stdexec::stoppable_token;
    using stdexec::stoppable_token_for;
    using stdexec::unstoppable_token;
#endif // #if(MLTVRS_EXECUTION_STOP_TOKEN_IMPL_STDEXEC)

} // namespace mltvrs
