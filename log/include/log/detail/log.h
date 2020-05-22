// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

log_level_t log_get_log_level();
log_msg_function log_get_log_msg_function();
log_convert_to_priority_function log_get_log_convert_to_priority_function();

void snprintf_xxd_buf(char* output, size_t size, const uint8_t* begin, const uint8_t* end);

#define DO_LOG_MESSAGE_NOCHECK_IMPL(logLevel, format, ...) \
    log_get_log_msg_function()(log_get_log_convert_to_priority_function()(logLevel), format, __VA_ARGS__)

#define DO_LOG_MESSAGE_IMPL(logLevel, format, ...)                  \
    do {                                                            \
        if (!(logLevel & log_get_log_level())) break;               \
        DO_LOG_MESSAGE_NOCHECK_IMPL(logLevel, format, __VA_ARGS__); \
    } while (0)

#define DO_LOG_XXD_MESSAGE_IMPL(logLevel, data, size, format, ...)                                 \
    do {                                                                                           \
        if (!(logLevel & log_get_log_level())) break;                                              \
                                                                                                   \
        size_t format_len = strlen(format);                                                        \
        size_t new_format_len = format_len + size * 3 + 1;                                         \
                                                                                                   \
        char* new_format = (char*)malloc(new_format_len);                                          \
        memset(new_format, 0, new_format_len);                                                     \
        memcpy(new_format, format, format_len);                                                    \
                                                                                                   \
        snprintf_xxd_buf(new_format + format_len, new_format_len - format_len, data, data + size); \
        DO_LOG_MESSAGE_NOCHECK_IMPL(logLevel, new_format, __VA_ARGS__);                            \
        free(new_format);                                                                          \
    } while (0)
