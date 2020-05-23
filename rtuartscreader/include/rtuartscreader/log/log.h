#pragma once

#include <log/log.h>

#define LOG_RV_NORETURN(logLevel, status, rv, convert_fn) \
    DO_LOG_MESSAGE(logLevel, "%s: %x (%s)", status, rv, convert_fn(rv))

#define LOG_RV_NORETURN_MSG(logLevel, status, rv, convert_fn, format, ...)   \
    do {                                                                     \
        DO_LOG_MESSAGE(logLevel, "%s: %x (%s)", status, rv, convert_fn(rv)); \
        DO_LOG_MESSAGE(logLevel, format, __VA_ARGS__);                       \
    } while (0)

#define LOG_RETURN_MSG(logLevel, status, rv, convert_fn, format, ...)               \
    do {                                                                            \
        LOG_RV_NORETURN_MSG(logLevel, status, rv, convert_fn, format, __VA_ARGS__); \
        return rv;                                                                  \
    } while (0)

#define LOG_RETURN(logLevel, status, rv, convert_fn)       \
    do {                                                   \
        LOG_RV_NORETURN(logLevel, status, rv, convert_fn); \
        return rv;                                         \
    } while (0)
