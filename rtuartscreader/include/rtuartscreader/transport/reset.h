#pragma once

#include <stddef.h>
#include <stdint.h>

#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/transport/transport_t.h>

transport_status_t transport_reset(transport_t* transport, uint8_t atr_buffer[], size_t* atr_len);
