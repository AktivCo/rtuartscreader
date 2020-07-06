// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/transport/transport_t.h>

#ifdef __cplusplus
extern "C" {
#endif

transport_status_t transport_reset(transport_t* transport, uint8_t atr_buffer[], size_t* atr_len);

#ifdef __cplusplus
}
#endif