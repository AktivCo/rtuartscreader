// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stdint.h>

#include <rtuartscreader/iso7816_3/f_d_index.h>
#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/transport/transport_t.h>

#ifdef __cplusplus
extern "C" {
#endif

iso7816_3_status_t do_pps_exchange(const transport_t* transport, const f_d_index_t* f_d_index, uint8_t protocol);

#ifdef __cplusplus
}
#endif
