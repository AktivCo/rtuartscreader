#pragma once

#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/transport/transport_t.h>

transport_status_t transport_initialize(transport_t* transport, const char* reader_name);

transport_status_t transport_reinitialize(transport_t* transport, const transmit_params_t* params);

transport_status_t transport_deinitialize(const transport_t* transport);
