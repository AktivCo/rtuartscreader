#pragma once

#include <stdint.h>

#include "transport.h"

typedef enum {
    transmit_status_ok = 0,
    transmit_status_communication_error,
    transmit_status_insufficient_buffer,
    transmit_status_invalid_params,
    transmit_status_protocol_error
} transmit_status_t;

transmit_status_t t0_transmit_apdu(const transport_t* transport, const uint8_t* tx_buf, uint16_t tx_len,
                                   uint8_t* rx_buf, uint16_t* rx_len);
