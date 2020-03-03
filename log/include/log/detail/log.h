// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

log_level_t log_get_log_level();
log_msg_function log_get_log_msg_function();
log_convert_to_priority_function log_get_log_convert_to_priority_function();

#define DO_LOG_MESSAGE_IMPL(logLevel, format, ...)                                                             \
    do {                                                                                                       \
        if (!(logLevel & log_get_log_level())) break;                                                          \
        log_get_log_msg_function()(log_get_log_convert_to_priority_function()(logLevel), format, __VA_ARGS__); \
    } while (0)
