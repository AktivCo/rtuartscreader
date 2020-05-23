#pragma once

#include <errno.h>

#include <rtuartscreader/hardware/hardware.h>
#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/log/log.h>
#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/utils/error.h>

#define LOG_RETURN_TRANSPORT_ERROR(rv) \
    LOG_RETURN(LOG_LEVEL_ERROR, "ERROR", rv, transport_status_to_string)

#define LOG_RETURN_TRANSPORT_ERROR_MSG(rv, format, ...) \
    LOG_RETURN_MSG(LOG_LEVEL_ERROR, "ERROR", rv, transport_status_to_string, format, __VA_ARGS__)

#define LOG_RETURN_ON_OS_ERROR(r) LOG_POPULATE_ERROR_MSG(r, 0, transport_status_os_error, transport_status_to_string, \
                                                         "OS error: %d, errno: %d", r, errno)

#define LOG_OS_ERROR(r) LOG_RV_NORETURN_MSG(LOG_LEVEL_ERROR, "ERROR", transport_status_os_error, transport_status_to_string, \
                                            "OS error: %d, errno: %d", r, errno)

#define RETURN_ON_HW_ERROR(r) POPULATE_ERROR(r, hw_status_ok, transport_status_hardware_error)

#define RETURN_ON_OS_ERROR(r) POPULATE_ERROR(r, 0, transport_status_os_error)

#define RETURN_ON_IS07816_3_ERROR(r) POPULATE_ERROR(r, iso7816_3_status_ok, transport_status_iso7816_3_error)
