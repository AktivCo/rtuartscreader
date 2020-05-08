#include "apdu_t0.h"

#include "transport.h"
#include "utils/buffer_view.h"

#define APDU_HEADER_SIZE 5

#define APDU_INS_OFFSET 1
#define APDU_P3_OFFSET 4

#define APDU_MAX_NE_VALUE 0x100

#define PROCEDURE_BYTE_NULL 0x60

#define RETURN_ON_TRANSPORT_ERROR(r)                    \
    do {                                                \
        if (r != transport_status_ok) {                 \
            return transmit_status_communication_error; \
        }                                               \
    } while (0)


static inline uint16_t le_to_ne(uint8_t le) {
    if (le == 0x00) return APDU_MAX_NE_VALUE;
    return le;
}

static transport_status_t send_apdu_header(const transport_t* transport, const uint8_t* tx_buf, uint8_t p3) {
    transport_status_t r;

    r = transport_send_bytes(transport, tx_buf, APDU_HEADER_SIZE - 1);
    if (r != transport_status_ok) {
        return r;
    }

    return transport_send_byte(transport, p3);
}

static transmit_status_t t0_transmit_data(const transport_t* transport, const uint8_t ack, pop_front_buffer_view* send_data,
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
                return transmit_status_protocol_error;
            }
            push_back_buffer_view_push(recv_data, proc_byte);

            if (push_back_buffer_view_full(recv_data)) {
                return transmit_status_protocol_error;
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
                    return transmit_status_protocol_error;
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
                    return transmit_status_protocol_error;
                }

                r = transport_recv_byte(transport, push_back_buffer_view_reserve_n(recv_data, 1));
                RETURN_ON_TRANSPORT_ERROR(r);
            }
        }
    }

    return transmit_status_ok;
}

// TODO: add logging for transport IO functions
transmit_status_t t0_transmit_apdu(const transport_t* transport, const uint8_t* tx_buf, uint16_t tx_len,
                                   uint8_t* rx_buf, uint16_t* rx_len) {
    transport_status_t r;

    uint16_t ne = 0; // expected data size to receive
    uint8_t nc = 0;  // expected data size to send

    uint8_t p3;

    if (tx_len < APDU_HEADER_SIZE - 1) {
        return transmit_status_invalid_params;
    } else if (tx_len == APDU_HEADER_SIZE - 1) {
        p3 = 0;
    } else if (tx_len == APDU_HEADER_SIZE) {
        p3 = tx_buf[APDU_P3_OFFSET];
        ne = le_to_ne(p3);
    } else if (tx_len > APDU_HEADER_SIZE) {
        p3 = tx_buf[APDU_P3_OFFSET];
        nc = p3;

        if (tx_len < APDU_HEADER_SIZE + nc) {
            return transmit_status_invalid_params;
        } else if (tx_len == nc + APDU_HEADER_SIZE) {
            ne = 0;
        } else if (tx_len == nc + APDU_HEADER_SIZE + 1) {
            ne = le_to_ne(tx_buf[tx_len - 1]);
        } else {
            return transmit_status_invalid_params;
        }
    }

    if (*rx_len < ne + 2) {
        return transmit_status_insufficient_buffer;
    }

    r = send_apdu_header(transport, tx_buf, p3);
    RETURN_ON_TRANSPORT_ERROR(r);

    pop_front_buffer_view send_data;
    pop_front_buffer_view_init(&send_data, tx_buf + APDU_HEADER_SIZE, nc);

    push_back_buffer_view recv_data;
    push_back_buffer_view_init(&recv_data, rx_buf, ne + 2);

    const uint8_t ack = tx_buf[APDU_INS_OFFSET];

    transmit_status_t transmit_data_result = t0_transmit_data(transport, ack, &send_data, &recv_data);
    if (transmit_data_result != transmit_status_ok) {
        return transmit_data_result;
    }

    *rx_len = push_back_buffer_view_size(&recv_data);

    return transmit_status_ok;
}
