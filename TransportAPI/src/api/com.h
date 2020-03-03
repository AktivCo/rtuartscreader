// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stdint.h>

#include <TransportAPI/types.h>

#ifdef __cplusplus
extern "C" {
#endif

io_status_t com_init(HANDLE *hcom, const char *com_name, uint32_t baudrate);

io_status_t com_deinit(HANDLE hcom);

#ifdef __cplusplus
}
#endif
