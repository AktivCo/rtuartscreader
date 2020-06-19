#pragma once

#include <stddef.h>
#include <stdint.h>

#include <rtuartscreader/transport/status.h>
#include <rtuartscreader/transport/transport_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFINE_FUNCTION(R, NAME, ARGS) R NAME ARGS;
#include <rtuartscreader/transport/detail/sendrecv_functions.h>
#undef DEFINE_FUNCTION

#define DEFINE_FUNCTION(R, NAME, ARGS) typedef R(*NAME##_fn) ARGS;
#include <rtuartscreader/transport/detail/sendrecv_functions.h>
#undef DEFINE_FUNCTION

typedef struct transport_sendrecv_impl {
#define DEFINE_FUNCTION(DUMMY1, NAME, DUMMY2) NAME##_fn NAME;
#include <rtuartscreader/transport/detail/sendrecv_functions.h>
#undef DEFINE_FUNCTION
} transport_sendrecv_impl_t;

void transport_set_sendrecv_impl(const transport_sendrecv_impl_t* impl);

void transport_reset_sendrecv_impl();

#ifdef __cplusplus
}
#endif