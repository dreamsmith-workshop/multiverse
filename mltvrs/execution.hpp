#pragma once

#if __has_include(<stdexec/execution.hpp>)
#include <stdexec/execution.hpp>
#define MLTVRS_EXECUTION_IMPL_STDEXEC true
#else
#define MLTVRS_EXECUTION_IMPL_STDEXEC false
#endif

namespace mltvrs::execution {

#if(MLTVRS_EXECUTION_IMPL_STDEXEC)
    using stdexec::as_awaitable;
    using stdexec::as_awaitable_t;
    using stdexec::bulk;
    using stdexec::bulk_t;
    using stdexec::completion_signatures;
    using stdexec::completion_signatures_of_t;
    using stdexec::connect;
    using stdexec::connect_result_t;
    using stdexec::connect_t;
    using stdexec::dependent_completion_signatures;
    using stdexec::ensure_started;
    using stdexec::ensure_started_t;
    using stdexec::env_of_t;
    using stdexec::error_types_of_t;
    using stdexec::execute;
    using stdexec::execute_t;
    using stdexec::forward_progress_guarantee;
    using stdexec::forwarding_scheduler_query;
    using stdexec::forwarding_scheduler_query_t;
    using stdexec::forwarding_sender_query;
    using stdexec::forwarding_sender_query_t;
    using stdexec::get_allocator;
    using stdexec::get_allocator_t;
    using stdexec::get_completion_scheduler;
    using stdexec::get_completion_scheduler_t;
    using stdexec::get_completion_signatures;
    using stdexec::get_completion_signatures_t;
    using stdexec::get_delegatee_scheduler;
    using stdexec::get_delegatee_scheduler_t;
    using stdexec::get_env;
    using stdexec::get_env_t;
    using stdexec::get_forward_progress_guarantee;
    using stdexec::get_forward_progress_guarantee_t;
    using stdexec::get_scheduler;
    using stdexec::get_scheduler_t;
    using stdexec::get_stop_token;
    using stdexec::get_stop_token_t;
    using stdexec::into_variant;
    using stdexec::into_variant_t;
    using stdexec::just;
    using stdexec::just_error;
    using stdexec::just_stopped;
    using stdexec::let_error;
    using stdexec::let_error_t;
    using stdexec::let_stopped;
    using stdexec::let_stopped_t;
    using stdexec::let_value;
    using stdexec::let_value_t;
    using stdexec::make_completion_signatures;
    using stdexec::no_env;
    using stdexec::on;
    using stdexec::on_t;
    using stdexec::operation_state;
    using stdexec::read;
    using stdexec::receiver;
    using stdexec::receiver_adaptor;
    using stdexec::receiver_of;
    using stdexec::run_loop;
    using stdexec::schedule;
    using stdexec::schedule_from;
    using stdexec::schedule_from_t;
    using stdexec::schedule_result_t;
    using stdexec::schedule_t;
    using stdexec::scheduler;
    using stdexec::sender;
    using stdexec::sender_adaptor_closure;
    using stdexec::sender_of;
    using stdexec::sender_to;
    using stdexec::sends_stopped;
    using stdexec::set_error;
    using stdexec::set_error_t;
    using stdexec::set_stopped;
    using stdexec::set_stopped_t;
    using stdexec::set_value;
    using stdexec::set_value_t;
    using stdexec::split;
    using stdexec::split_t;
    using stdexec::start;
    using stdexec::start_detached;
    using stdexec::start_detached_t;
    using stdexec::start_t;
    using stdexec::stop_token_of_t;
    using stdexec::stopped_as_error;
    using stdexec::stopped_as_error_t;
    using stdexec::stopped_as_optional;
    using stdexec::stopped_as_optional_t;
    using stdexec::then;
    using stdexec::then_t;
    using stdexec::transfer;
    using stdexec::transfer_just;
    using stdexec::transfer_just_t;
    using stdexec::transfer_t;
    using stdexec::transfer_when_all;
    using stdexec::transfer_when_all_t;
    using stdexec::transfer_when_all_with_variant;
    using stdexec::transfer_when_all_with_variant_t;
    using stdexec::upon_error;
    using stdexec::upon_error_t;
    using stdexec::value_types_of_t;
    using stdexec::when_all;
    using stdexec::when_all_t;
    using stdexec::when_all_with_variant;
    using stdexec::when_all_with_variant_t;
    using stdexec::with_awaitable_senders;
#endif // #if(MLTVRS_EXECUTION_IMPL_STDEXEC)

} // namespace mltvrs::execution

namespace mltvrs::this_thread {

#if(MLTVRS_EXECUTION_IMPL_STDEXEC)
    using stdexec::execute_may_block_caller;
    using stdexec::execute_may_block_caller_t;
    using stdexec::sync_wait;
    using stdexec::sync_wait_t;
    using stdexec::sync_wait_with_variant;
    using stdexec::sync_wait_with_variant_t;
#endif // #if(MLTVRS_EXECUTION_IMPL_STDEXEC)

} // namespace mltvrs::this_thread
