#pragma once

#include <stdint.h>

#include <rtuartscreader/iso7816_3/f_d_index.h>
#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/transport/transport_t.h>

iso7816_3_status_t do_pps_exchange(const transport_t* transport, const f_d_index_t* f_d_index, uint8_t protocol);