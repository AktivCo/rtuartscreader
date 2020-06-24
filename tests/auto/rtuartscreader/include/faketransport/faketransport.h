#pragma once

#include <memory>

namespace rt {
namespace faketransport {

void initializeTransport();
void deinitializeTransport();

void initializeSendRecv();
void deinitializeSendRecv();

void initializeInitialize();
void deinitializeInitialize();

} // namespace faketransport
} // namespace rt