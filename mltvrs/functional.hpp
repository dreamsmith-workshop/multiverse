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

    /**
     * @brief An overload set that can be created inline from the given function objects.
     *
     * This template implements `std::overload` from the ISO WG21 proposal
     * [`P0051R3`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0051r3.pdf).
     *
     * @tparam F The types of function objects to form an overload set from.
     */
    template<typename... F>
    struct overload;

    template<typename F1, typename F2>
    struct overload<F1, F2> : public F1, F2
    {
        public:
            constexpr overload(F1 f1, F2 f2) noexcept : F1{std::move(f1)}, F2{std::move(f2)} {}

            using F1::operator();
            using F2::operator();
    };

    template<typename F1, typename F2, typename F3, typename... FN>
    struct overload<F1, F2, F3, FN...> : public overload<F2, F3, FN...>
    {
        public:
            constexpr overload(F1 f1, F2 f2, F3 f3, FN... fn) noexcept
                : F1{std::move(f1)},
                  overload<F2, F3, FN...>{std::move(f2), std::move(f3), std::move(fn)...}
            {
            }

            using F1::operator();
            using overload<F2, F3, FN...>::operator();
    };

    template<typename... F>
    overload(F&&...) -> overload<std::remove_cvref_t<F>...>;

} // namespace mltvrs
