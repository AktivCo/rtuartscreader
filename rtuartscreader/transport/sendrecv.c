// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/transport/sendrecv.h>

#include <fcntl.h>
#include <unistd.h>

#include <rtuartscreader/transport/detail/error.h>

// Read timeout is expected to be set based on WT value
// derived from selected transport parameters during PPS.
static transport_status_t do_transport_recv_byte_impl(const transport_t* transport, uint8_t* byte) {
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
static transport_status_t do_transport_send_byte_impl(const transport_t* transport, uint8_t byte) {
    uint8_t echo;

    if (write(transport->handle, &byte, 1) != 1)
        return transport_status_communication_error;

    // handle synchronous echo byte
    return do_transport_recv_byte_impl(transport, &echo);
}

static transport_status_t transport_recv_bytes_impl(const transport_t* transport, uint8_t* buf, size_t len) {
    transport_status_t r = transport_status_ok;
    size_t recv;

    for (recv = 0; recv < len; ++recv) {
        r = do_transport_recv_byte_impl(transport, &buf[recv]);
        if (r != transport_status_ok) {
            LOG_RETURN_TRANSPORT_ERROR(r);
        }
    }

    LOG_XXD_INFO(buf, len, "recv: ");

    return r;
}

static transport_status_t transport_send_bytes_impl(const transport_t* transport, const uint8_t* bytes, size_t len) {
    LOG_XXD_INFO(bytes, len, "send: ");

    transport_status_t r = transport_status_ok;
    size_t sent;

    for (sent = 0; sent != len; ++sent) {
        r = do_transport_send_byte_impl(transport, bytes[sent]);
        if (r != transport_status_ok) {
            LOG_RETURN_TRANSPORT_ERROR(r);
        }
    }

    return r;
}

static transport_status_t transport_recv_byte_impl(const transport_t* transport, uint8_t* byte) {
    return transport_recv_bytes_impl(transport, byte, 1);
}

static transport_status_t transport_send_byte_impl(const transport_t* transport, uint8_t byte) {
    return transport_send_bytes_impl(transport, &byte, 1);
}

#define PIMPL_NAME_PREFIX transport_sendrecv
#define PIMPL_FUNCTIONS_DECLARATION_PATH <rtuartscreader/transport/detail/sendrecv_functions.h>
#include <rtuartscreader/pimpl/source.h>
