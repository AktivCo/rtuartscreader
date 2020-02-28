#include <log/log.h>

#include <stdbool.h>

void dummy_log_msg(const int priority, const char *fmt, ...);
int dummy_log_convert_to_priority(log_level_t logLevel);

static log_level_t gLogLevel = LOG_LEVEL_NONE;
static log_msg_function gLogMsgFunction = dummy_log_msg;
static log_convert_to_priority_function gLogConvertToPriorityFunction = dummy_log_convert_to_priority;

void dummy_log_msg(const int priority, const char *fmt, ...) {
    (void)priority;
    (void)fmt;
}

int dummy_log_convert_to_priority(log_level_t logLevel) {
    (void)logLevel;
    return 0;
}

void log_init(log_level_t logLevel, log_msg_function msgFunction,
              log_convert_to_priority_function convertToPriorityFunction) {
    gLogLevel = logLevel;
    gLogMsgFunction = msgFunction;
    gLogConvertToPriorityFunction = convertToPriorityFunction;
}

log_level_t log_get_log_level() {
    return gLogLevel;
}

log_msg_function log_get_log_msg_function() {
    return gLogMsgFunction;
}

log_convert_to_priority_function log_get_log_convert_to_priority_function() {
    return gLogConvertToPriorityFunction;
}