// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

typedef enum {
    iso7816_3_status_ok = 0,
    iso7816_3_status_communication_error,
    iso7816_3_status_insufficient_buffer,
    iso7816_3_status_invalid_params,
    iso7816_3_status_unexpected_card_response,
    iso7816_3_status_pps_exchange_failed,
    iso7816_3_status_pps_exchange_use_default_f_d
} iso7816_3_status_t;

const char* iso7816_3_status_to_string(iso7816_3_status_t status);
