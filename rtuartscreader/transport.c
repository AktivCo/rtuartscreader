#include "transport.h"

#include <stddef.h>
#include <unistd.h>

#include "log.h"

// Read timeout is expected to be set based on WT value
// derived from selected transport parameters during PPS.
transport_status_t transport_recv_byte(const transport_t* transport, uint8_t* byte) {
    ssize_t rsize = read(transport->handle, byte, 1);

    if (rsize == -1) {
        return transport_status_communication_error;
    }

    if (!rsize) {
        return transport_status_timeout;
    }

    return transport_status_ok;
}

// TODO: support extra guard time
transport_status_t transport_send_byte(const transport_t* transport, uint8_t byte) {
    uint8_t echo;

    if (write(transport->handle, &byte, 1) != 1)
        return transport_status_communication_error;

    // handle synchronous echo byte
    return transport_recv_byte(transport, &echo);
}

transport_status_t transport_recv_bytes(const transport_t* transport, uint8_t* buf, size_t len) {
    transport_status_t r = transport_status_ok;
    size_t recv;

    for (recv = 0; recv < len; ++recv) {
        r = transport_recv_byte(transport, &buf[recv]);
        if (r != transport_status_ok) {
            LOG_ERROR("transport_recv_byte failed: %d", r);
            break;
        }
    }

    return r;
}

transport_status_t transport_send_bytes(const transport_t* transport, const uint8_t* bytes, size_t len) {
    transport_status_t r = transport_status_ok;
    size_t sent;

    for (sent = 0; sent != len; ++sent) {
        r = transport_send_byte(transport, bytes[sent]);
        if (r != transport_status_ok) {
            LOG_ERROR("transport_send_byte failed: %d", r);
            break;
        }
    }

    return r;
}
