#pragma once

#include <stdint.h>

#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/transport/transport_t.h>

#ifdef __cplusplus
extern "C" {
#endif

iso7816_3_status_t t0_transmit_apdu(const transport_t* transport, const uint8_t* tx_buf, uint16_t tx_len,
                                    uint8_t* rx_buf, uint16_t* rx_len);

#ifdef __cplusplus
}
#endif