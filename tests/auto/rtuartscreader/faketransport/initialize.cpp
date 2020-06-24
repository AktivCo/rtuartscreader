#include <faketransport/initialize.h>

#include <memory>

#include <rtuartscreader/transport/initialize.h>

using namespace std;

unique_ptr<rt::faketransport::Initialize> gFakeInitialize = nullptr;

transport_status_t transport_initialize_impl(transport_t* transport, const char* name) {
    if (!gFakeInitialize) throw runtime_error("You need to set faketransport::Initialize object");
    return gFakeInitialize->transport_initialize(transport, name);
}

transport_status_t transport_reinitialize_impl(transport_t* transport, const transmit_params_t* params) {
    if (!gFakeInitialize) throw runtime_error("You need to set faketransport::Initialize object");
    return gFakeInitialize->transport_reinitialize(transport, params);
}

transport_status_t transport_deinitialize_impl(const transport_t* transport) {
    if (!gFakeInitialize) throw runtime_error("You need to set faketransport::Initialize object");
    return gFakeInitialize->transport_deinitialize(transport);
}

transport_initialize_impl_t gTransportInitializeImpl = {
    .transport_initialize = transport_initialize_impl,
    .transport_reinitialize = transport_reinitialize_impl,
    .transport_deinitialize = transport_deinitialize_impl
};

namespace rt {
namespace faketransport {

void initializeInitialize() {
    transport_initialize_impl_set(&gTransportInitializeImpl);
}
void deinitializeInitialize() {
    transport_initialize_impl_reset();
}

void setInitialize(unique_ptr<rt::faketransport::Initialize>&& initialize) {
    gFakeInitialize = move(initialize);
}

void resetInitialize() {
    gFakeInitialize = nullptr;
}

} // namespace faketransport
} // namespace rt