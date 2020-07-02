// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.
#pragma once

#include <rtuartscreader/log/log.h>

#define LOG_CRITICAL_RETURN_IFD(error, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_CRITICAL, "ERROR", error, ifd_error_to_string, format, __VA_ARGS__)

#define LOG_ERROR_RETURN_IFD(error, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_ERROR, "ERROR", error, ifd_error_to_string, format, __VA_ARGS__)

#define LOG_INFO_RETURN_IFD(rv) \
    LOG_RETURN(LOG_LEVEL_INFO, "OK", rv, ifd_error_to_string)

static const char* ifd_error_to_string(int error);
