#pragma once

typedef enum {
    iso7816_3_status_ok = 0,
    iso7816_3_status_communication_error,
    iso7816_3_status_insufficient_buffer,
    iso7816_3_status_invalid_params,
    iso7816_3_status_unexpected_card_response,
    iso7816_3_status_pps_exchange_failed
} iso7816_3_status_t;