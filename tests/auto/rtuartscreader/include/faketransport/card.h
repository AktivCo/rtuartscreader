// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <rtuartscreader/transport/sendrecv.h>

namespace rt {
namespace faketransport {

class Card {
public:
    virtual void input(const uint8_t* buffer, size_t length) = 0;

    virtual void output(uint8_t* buffer, size_t length) = 0;

    virtual ~Card() = default;
};

void setCard(const std::shared_ptr<Card>& card);
void resetCard();

} // namespace faketransport
} // namespace rt
