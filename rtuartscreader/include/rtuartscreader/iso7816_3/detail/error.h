// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/log/log.h>
#include <rtuartscreader/utils/error.h>

#define LOG_RETURN_ISO7816_3_ERROR(rv) \
    LOG_RETURN(LOG_LEVEL_ERROR, "ERROR", rv, iso7816_3_status_to_string)

#define LOG_RETURN_ISO7816_3_ERROR_MSG(rv, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_ERROR, "ERROR", rv, iso7816_3_status_to_string, format, __VA_ARGS__)

#define RETURN_ON_TRANSPORT_ERROR(r) POPULATE_ERROR(r, transport_status_ok, iso7816_3_status_communication_error)
