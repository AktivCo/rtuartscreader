#pragma once

#include <rtuartscreader/log/log.h>

#define POPULATE_ERROR(TESTED_VALUE, PRIVATE_EXPECTED_VALUE, PUBLIC_ERROR_VALUE) \
    do {                                                                         \
        if ((TESTED_VALUE) != PRIVATE_EXPECTED_VALUE) {                          \
            return PUBLIC_ERROR_VALUE;                                           \
        }                                                                        \
    } while (0)

#define LOG_POPULATE_ERROR_MSG(TESTED_VALUE, PRIVATE_EXPECTED_VALUE, PUBLIC_ERROR_VALUE, STRINGIFY_FN, FORMAT, ...) \
    do {                                                                                                            \
        if ((TESTED_VALUE) != PRIVATE_EXPECTED_VALUE) {                                                             \
            LOG_RETURN_MSG(LOG_LEVEL_ERROR, "ERROR", PUBLIC_ERROR_VALUE, STRINGIFY_FN, FORMAT, __VA_ARGS__);        \
        }                                                                                                           \
    } while (0)
