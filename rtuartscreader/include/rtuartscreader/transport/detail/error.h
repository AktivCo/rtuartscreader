#pragma once

#include <rtuartscreader/hardware/hardware.h>
#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/utils/error.h>

#define RETURN_ON_HW_ERROR(r) POPULATE_ERROR(r, hw_status_ok, transport_status_hardware_error)

#define RETURN_ON_OS_ERROR(r) POPULATE_ERROR(r, 0, transport_status_os_error)

#define RETURN_ON_IS07816_3_ERROR(r) POPULATE_ERROR(r, iso7816_3_status_ok, transport_status_iso7816_3_error)
