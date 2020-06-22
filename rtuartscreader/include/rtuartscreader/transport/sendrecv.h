#pragma once

#include <stddef.h>
#include <stdint.h>

#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/transport/transport_t.h>

#define PIMPL_NAME_PREFIX transport_sendrecv
#define PIMPL_FUNCTIONS_DECLARATION_PATH <rtuartscreader/transport/detail/sendrecv_functions.h>
#include <rtuartscreader/pimpl/header.h>
