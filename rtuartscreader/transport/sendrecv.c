#include <rtuartscreader/transport/sendrecv.h>

#include <fcntl.h>
#include <unistd.h>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>

#include <rtuartscreader/log.h>

// Read timeout is expected to be set based on WT value
// derived from selected transport parameters during PPS.
static transport_status_t transport_recv_byte_impl(const transport_t* transport, uint8_t* byte) {
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
static transport_status_t transport_send_byte_impl(const transport_t* transport, uint8_t byte) {
    uint8_t echo;

    if (write(transport->handle, &byte, 1) != 1)
        return transport_status_communication_error;

    // handle synchronous echo byte
    return transport_recv_byte(transport, &echo);
}

static transport_status_t transport_recv_bytes_impl(const transport_t* transport, uint8_t* buf, size_t len) {
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

static transport_status_t transport_send_bytes_impl(const transport_t* transport, const uint8_t* bytes, size_t len) {
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

transport_sendrecv_impl_t g_sendrecv_default_impl = {
#define DEFINE_FUNCTION(DUMMY1, NAME, DUMMY2) .NAME = BOOST_PP_CAT(NAME, _impl),
#include <rtuartscreader/transport/detail/sendrecv_functions.h>
#undef DEFINE_FUNCTION
};

const transport_sendrecv_impl_t* g_sendrecv_impl = &g_sendrecv_default_impl;


#define EXPAND_ONE_ARG(Z, N, ARGS)  \
    BOOST_PP_TUPLE_ELEM(Z, N, ARGS) \
    arg##N
#define EXPAND_ARGS(ARGS) BOOST_PP_ENUM(BOOST_PP_TUPLE_SIZE(ARGS), EXPAND_ONE_ARG, ARGS)

#define EXPAND_PARAMS(ARGS) BOOST_PP_ENUM_PARAMS(BOOST_PP_TUPLE_SIZE(ARGS), arg)

#define DEFINE_FUNCTION(R, NAME, ARGS)                     \
    R NAME(EXPAND_ARGS(ARGS)) {                            \
        return g_sendrecv_impl->NAME(EXPAND_PARAMS(ARGS)); \
    }

#include <rtuartscreader/transport/detail/sendrecv_functions.h>

#undef EXPAND_ONE_ARG
#undef EXPAND_ARGS
#undef EXPAND_PARAMS
#undef DEFINE_FUNCTION

void transport_set_sendrecv_impl(const transport_sendrecv_impl_t* impl) {
    g_sendrecv_impl = impl;
}

void transport_reset_sendrecv_impl() {
    g_sendrecv_impl = &g_sendrecv_default_impl;
}
