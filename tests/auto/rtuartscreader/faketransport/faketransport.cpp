#include <faketransport/faketransport.h>

namespace rt {
namespace faketransport {

void initializeTransport() {
    initializeSendRecv();
    initializeInitialize();
}
void deinitializeTransport() {
    deinitializeInitialize();
    deinitializeSendRecv();
}


} // namespace faketransport
} // namespace rt