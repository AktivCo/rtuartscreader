#pragma once

log_level_t log_get_log_level();
log_msg_function log_get_log_msg_function();
log_convert_to_priority_function log_get_log_convert_to_priority_function();

#define DO_LOG_MESSAGE_IMPL(logLevel, format, ...)                                                             \
    do {                                                                                                       \
        if (!(logLevel & log_get_log_level())) break;                                                          \
        log_get_log_msg_function()(log_get_log_convert_to_priority_function()(logLevel), format, __VA_ARGS__); \
    } while (0)
