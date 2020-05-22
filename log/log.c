// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <log/log.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static void dummy_log_msg(const int priority, const char* fmt, ...);
static int dummy_log_convert_to_priority(log_level_t logLevel);

static log_level_t gLogLevel = LOG_LEVEL_NONE;
static log_msg_function gLogMsgFunction = dummy_log_msg;
static log_convert_to_priority_function gLogConvertToPriorityFunction = dummy_log_convert_to_priority;

static void dummy_log_msg(const int priority, const char* fmt, ...) {
    (void)priority;
    (void)fmt;
}

static int dummy_log_convert_to_priority(log_level_t logLevel) {
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

void snprintf_xxd_buf(char* output, size_t size, const uint8_t* begin, const uint8_t* end) {
    memset(output, 0, size);

    assert(size < INT_MAX);
    int ssize = (int)size;

    for (const uint8_t* it = begin; it != end; ++it) {
        size_t n = snprintf(output, ssize, "%02X ", *it);
        output += n;
        ssize -= n;
        if (ssize <= 0) return;
    }
}
