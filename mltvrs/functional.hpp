#pragma once

#include <functional>

#if __has_include(<stdexec/functional.hpp>)
#include <stdexec/functional.hpp>
#define MLTVRS_EXECUTION_FUNCTIONAL_IMPL_STDEXEC true
#else
#define MLTVRS_EXECUTION_FUNCTIONAL_IMPL_STDEXEC false
#endif

namespace mltvrs {

#if(MLTVRS_EXECUTION_FUNCTIONAL_IMPL_STDEXEC)
    using stdexec::nothrow_tag_invocable;
    using stdexec::tag_invocable;
    using stdexec::tag_invoke;
    using stdexec::tag_invoke_result;
    using stdexec::tag_invoke_result_t;
    using stdexec::tag_t;
#endif // #if(MLTVRS_EXECUTION_FUNCTIONAL_IMPL_STDEXEC)

} // namespace mltvrs
