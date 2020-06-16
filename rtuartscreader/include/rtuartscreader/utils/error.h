#pragma once

#define POPULATE_ERROR(TESTED_VALUE, PRIVATE_EXPECTED_VALUE, PUBLIC_ERROR_VALUE) \
    do {                                                                         \
        if ((TESTED_VALUE) != PRIVATE_EXPECTED_VALUE) {                          \
            return PUBLIC_ERROR_VALUE;                                           \
        }                                                                        \
    } while (0)
