// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <memory>

#include <rtuartscreader/transport/transport.h>

namespace rt {
namespace faketransport {

class Initialize {
public:
    virtual transport_status_t transport_initialize(transport_t* transport, const char* name) = 0;
    virtual transport_status_t transport_reinitialize(transport_t* transport, const transmit_params_t* params) = 0;
    virtual transport_status_t transport_deinitialize(const transport_t* transport) = 0;
    virtual ~Initialize() = default;
};

void setInitialize(std::unique_ptr<Initialize>&& initialize);
void resetInitialize();

} // namespace faketransport
} // namespace rt
