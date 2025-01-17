// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

typedef enum {
    transport_status_ok = 0,
    transport_status_timeout,
    transport_status_communication_error,
    transport_status_os_error,
    transport_status_hardware_error,
    transport_status_iso7816_3_error,
    transport_status_invalid_atr,
    transport_status_mode_not_supported,
    transport_status_need_reset
} transport_status_t;

const char* transport_status_to_string(transport_status_t status);
