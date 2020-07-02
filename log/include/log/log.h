// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_CRITICAL = 0x01,
    LOG_LEVEL_ERROR = 0x01 << 1,
    LOG_LEVEL_INFO = 0x01 << 2,
    LOG_LEVEL_PERIODIC = 0x01 << 3
} log_level_t;

#define LOG_CRITICAL(format, ...) \
    DO_LOG_MESSAGE(LOG_LEVEL_CRITICAL, format, __VA_ARGS__)

#define LOG_ERROR(format, ...) \
    DO_LOG_MESSAGE(LOG_LEVEL_ERROR, format, __VA_ARGS__)

#define LOG_INFO(format, ...) \
    DO_LOG_MESSAGE(LOG_LEVEL_INFO, format, __VA_ARGS__)

#define LOG_XXD_INFO(data, data_size, format, ...) \
    DO_LOG_XXD_MESSAGE(LOG_LEVEL_INFO, data, data_size, format, __VA_ARGS__)

// This is gcc magic, probably won't work with other compilers
#define VA_ARGS(...) , ##__VA_ARGS__

#define DO_LOG_MESSAGE(logLevel, format, ...) \
    DO_LOG_MESSAGE_IMPL(logLevel, "%s:%d:%s() " format, __FILE__, __LINE__, __FUNCTION__ VA_ARGS(__VA_ARGS__))

#define DO_LOG_XXD_MESSAGE(logLevel, data, data_size, format, ...) \
    DO_LOG_XXD_MESSAGE_IMPL(logLevel, data, data_size, "%s:%d:%s() " format, __FILE__, __LINE__, __FUNCTION__ VA_ARGS(__VA_ARGS__))

typedef void (*log_msg_function)(const int priority, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

typedef int (*log_convert_to_priority_function)(log_level_t);

void log_init(log_level_t logLevel, log_msg_function msgFunction, log_convert_to_priority_function convertToPriorityFunction);

log_level_t log_get_log_level();

void log_set_log_level(log_level_t logLevel);

#include "detail/log.h"

#ifdef __cplusplus
}
#endif
