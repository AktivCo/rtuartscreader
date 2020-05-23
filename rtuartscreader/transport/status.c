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
