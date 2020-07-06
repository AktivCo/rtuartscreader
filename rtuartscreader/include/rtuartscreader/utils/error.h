// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

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
