// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/transport/status.h>

const char* transport_status_to_string(transport_status_t status) {
    switch (status) {
    case transport_status_ok: return "transport_status_ok";
    case transport_status_timeout: return "transport_status_timeout";
    case transport_status_communication_error: return "transport_status_communication_error";
    case transport_status_os_error: return "transport_status_os_error";
    case transport_status_hardware_error: return "transport_status_hardware_error";
    case transport_status_iso7816_3_error: return "transport_status_iso7816_3_error";
    case transport_status_invalid_atr: return "transport_status_invalid_atr";
    case transport_status_mode_not_supported: return "transport_status_mode_not_supported";
    case transport_status_need_reset: return "transport_status_need_reset";
    }

    return "unknown";
}
