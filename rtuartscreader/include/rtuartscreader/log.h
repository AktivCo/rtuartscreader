// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once
#include <log/log.h>

#define LOG_RETURN_MSG(logLevel, status, rv, format, ...)                             \
    do {                                                                              \
        DO_LOG_MESSAGE(logLevel, "%s: %x (%s)", status, rv, ifd_error_to_string(rv)); \
        DO_LOG_MESSAGE(logLevel, format, __VA_ARGS__);                                \
        return rv;                                                                    \
    } while (0)

#define LOG_RETURN(logLevel, status, rv)                                              \
    do {                                                                              \
        DO_LOG_MESSAGE(logLevel, "%s: %x (%s)", status, rv, ifd_error_to_string(rv)); \
        return rv;                                                                    \
    } while (0)

#define LOG_CRITICAL_RETURN_IFD(error, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_CRITICAL, "ERROR", error, format, __VA_ARGS__)

#define LOG_ERROR_RETURN_IFD(error, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_ERROR, "ERROR", error, format, __VA_ARGS__)

#define LOG_INFO_RETURN_IFD(rv) \
    LOG_RETURN(LOG_LEVEL_INFO, "OK", rv)

#define LOG_DEBUG_RETURN_IFD(rv) \
    LOG_RETURN(LOG_LEVEL_DEBUG, "OK", rv)

void init_log();

const char* ifd_error_to_string(int error);
