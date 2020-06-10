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
