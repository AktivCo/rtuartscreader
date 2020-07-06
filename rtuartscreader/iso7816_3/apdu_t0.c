// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/iso7816_3/apdu_t0.h>

#include <rtuartscreader/iso7816_3/detail/error.h>
#include <rtuartscreader/log/log.h>
#include <rtuartscreader/transport/sendrecv.h>
#include <rtuartscreader/transport/transport_t.h>
#include <rtuartscreader/utils/buffer_view.h>

#define APDU_HEADER_SIZE 5

#define APDU_INS_OFFSET 1
#define APDU_P3_OFFSET 4

#define APDU_MAX_NE_VALUE 0x100

#define PROCEDURE_BYTE_NULL 0x60

static inline uint16_t le_to_ne(uint8_t le) {
    if (le == 0x00) return APDU_MAX_NE_VALUE;
    return le;
}

static transport_status_t send_apdu_header(const transport_t* transport, const uint8_t* tx_buf, uint8_t p3) {
    transport_status_t r;

    r = transport_send_bytes(transport, tx_buf, APDU_HEADER_SIZE - 1);
    POPULATE_ERROR(r, transport_status_ok, r);

    return transport_send_byte(transport, p3);
}

static iso7816_3_status_t t0_transceive_data(const transport_t* transport, const uint8_t ack, pop_front_buffer_view* send_data,
                                             push_back_buffer_view* recv_data) {
    const uint8_t inv_ack = ~ack;

    while (1) {
        uint8_t proc_byte;

        transport_status_t r = transport_recv_byte(transport, &proc_byte);
        RETURN_ON_TRANSPORT_ERROR(r);

        // NULL byte
        if (proc_byte == PROCEDURE_BYTE_NULL)
            continue;

        // SW1 byte, recieve SW2 and quit
        if ((proc_byte & 0xf0) == 0x60 || (proc_byte & 0xf0) == 0x90) {
            if (push_back_buffer_view_full(recv_data)) {
                LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_unexpected_card_response,
                                               "The card sent more data than expected");
            }
            push_back_buffer_view_push(recv_data, proc_byte);

            if (push_back_buffer_view_full(recv_data)) {
                LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_unexpected_card_response,
                                               "The card sent more data than expected");
            }

            r = transport_recv_byte(transport, push_back_buffer_view_reserve_n(recv_data, 1));
            RETURN_ON_TRANSPORT_ERROR(r);

            break;
        }

        if (proc_byte == ack) {
            if (!pop_front_buffer_view_empty(send_data)) {
                size_t data_size = pop_front_buffer_view_size(send_data);
                const uint8_t* data = pop_front_buffer_view_pop_n(send_data, data_size);

                r = transport_send_bytes(transport, data, data_size);
                RETURN_ON_TRANSPORT_ERROR(r);
            } else {
                size_t free_space = push_back_buffer_view_free_space(recv_data);
                if (free_space <= 2) {
                    LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_unexpected_card_response,
                                                   "The card sent more data than expected");
                }

                uint8_t* data = push_back_buffer_view_reserve_n(recv_data, free_space - 2);

                r = transport_recv_bytes(transport, data, free_space - 2);
                RETURN_ON_TRANSPORT_ERROR(r);
            }

            continue;
        }

        if (proc_byte == inv_ack) {
            if (!pop_front_buffer_view_empty(send_data)) {
                r = transport_send_byte(transport, pop_front_buffer_view_pop(send_data));
                RETURN_ON_TRANSPORT_ERROR(r);
            } else {
                size_t free_space = push_back_buffer_view_free_space(recv_data);
                if (free_space <= 2) {
                    LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_unexpected_card_response,
                                                   "The card sent more data than expected");
                }

                r = transport_recv_byte(transport, push_back_buffer_view_reserve_n(recv_data, 1));
                RETURN_ON_TRANSPORT_ERROR(r);
            }
        }
    }

    return iso7816_3_status_ok;
}

// TODO: add logging for transport IO functions
iso7816_3_status_t t0_transmit_apdu(const transport_t* transport, const uint8_t* tx_buf, uint16_t tx_len,
                                    uint8_t* rx_buf, uint16_t* rx_len) {
    transport_status_t r;

    uint16_t ne = 0; // expected data size to receive
    uint8_t nc = 0;  // expected data size to send

    uint8_t p3;

    if (tx_len < APDU_HEADER_SIZE - 1) {
        LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_invalid_params, "APDU buffer too short");
    } else if (tx_len == APDU_HEADER_SIZE - 1) {
        p3 = 0;
    } else if (tx_len == APDU_HEADER_SIZE) {
        p3 = tx_buf[APDU_P3_OFFSET];
        ne = le_to_ne(p3);
    } else if (tx_len > APDU_HEADER_SIZE) {
        p3 = tx_buf[APDU_P3_OFFSET];
        nc = p3;

        if (tx_len < APDU_HEADER_SIZE + nc) {
            LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_invalid_params, "APDU is not complete");
        } else if (tx_len == nc + APDU_HEADER_SIZE) {
            ne = 0;
        } else if (tx_len == nc + APDU_HEADER_SIZE + 1) {
            ne = le_to_ne(tx_buf[tx_len - 1]);
        } else {
            LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_invalid_params, "APDU buffer has excess data");
        }
    }

    if (*rx_len < ne + 2) {
        LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_insufficient_buffer, "Response buffer too short");
    }

    r = send_apdu_header(transport, tx_buf, p3);
    RETURN_ON_TRANSPORT_ERROR(r);

    pop_front_buffer_view send_data;
    pop_front_buffer_view_init(&send_data, tx_buf + APDU_HEADER_SIZE, nc);

    push_back_buffer_view recv_data;
    push_back_buffer_view_init(&recv_data, rx_buf, ne + 2);

    const uint8_t ack = tx_buf[APDU_INS_OFFSET];

    iso7816_3_status_t transceive_data_result = t0_transceive_data(transport, ack, &send_data, &recv_data);
    POPULATE_ERROR(transceive_data_result, iso7816_3_status_ok, transceive_data_result);

    *rx_len = push_back_buffer_view_size(&recv_data);

    return iso7816_3_status_ok;
}
