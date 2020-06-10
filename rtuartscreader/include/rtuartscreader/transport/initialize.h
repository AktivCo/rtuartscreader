#pragma once

#include "transport_status_t.h"
#include "transport_t.h"

transport_status_t transport_initialize(transport_t* transport, const char* reader_name);

transport_status_t transport_deinitialize(const transport_t* transport);
