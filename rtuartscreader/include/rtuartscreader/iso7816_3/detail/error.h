#pragma once

#include <rtuartscreader/utils/error.h>

#define RETURN_ON_TRANSPORT_ERROR(r) POPULATE_ERROR(r, transport_status_ok, iso7816_3_status_communication_error)
