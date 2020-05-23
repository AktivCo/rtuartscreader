#include <rtuartscreader/iso7816_3/status.h>

const char* iso7816_3_status_to_string(iso7816_3_status_t status) {
    switch (status) {
    case iso7816_3_status_ok: return "iso7816_3_status_ok";
    case iso7816_3_status_communication_error: return "iso7816_3_status_communication_error";
    case iso7816_3_status_insufficient_buffer: return "iso7816_3_status_insufficient_buffer";
    case iso7816_3_status_invalid_params: return "iso7816_3_status_invalid_params";
    case iso7816_3_status_unexpected_card_response: return "iso7816_3_status_unexpected_card_response";
    case iso7816_3_status_pps_exchange_failed: return "iso7816_3_status_pps_exchange_failed";
    }

    return "unknown";
}
