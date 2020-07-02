#pragma once

#include <stdint.h>

typedef enum {
    hw_status_ok = 0,
    hw_status_failed
} hw_status_t;

#define PIMPL_NAME_PREFIX hw
#define PIMPL_FUNCTIONS_DECLARATION_PATH <rtuartscreader/hardware/detail/hardware_functions.h>
#include <rtuartscreader/pimpl/header.h>
