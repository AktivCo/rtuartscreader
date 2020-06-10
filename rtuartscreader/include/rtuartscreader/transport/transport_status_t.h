#pragma once

typedef enum {
    transport_status_ok = 0,
    transport_status_timeout,
    transport_status_communication_error,
    transport_status_os_error,
    transport_status_hardware_error
} transport_status_t;
