#pragma once

#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/log/log.h>
#include <rtuartscreader/utils/error.h>

#define LOG_RETURN_ISO7816_3_ERROR(rv) \
    LOG_RETURN(LOG_LEVEL_ERROR, "ERROR", rv, iso7816_3_status_to_string)

#define LOG_RETURN_ISO7816_3_ERROR_MSG(rv, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_ERROR, "ERROR", rv, iso7816_3_status_to_string, format, __VA_ARGS__)

#define RETURN_ON_TRANSPORT_ERROR(r) POPULATE_ERROR(r, transport_status_ok, iso7816_3_status_communication_error)
