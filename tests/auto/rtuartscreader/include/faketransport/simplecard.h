#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <faketransport/card.h>

namespace rt {
namespace faketransport {

class SimpleCard : public Card {
public:
    SimpleCard(std::vector<uint8_t> output)
        : mOutput(move(output))
        , mOutputBytesHeadOffset(0) {}

    virtual void input(const uint8_t* buffer, size_t length) override;

    virtual void output(uint8_t* buffer, size_t length) override;

    const std::vector<uint8_t>& getInput() const;

    std::vector<uint8_t> getTransmittedOutput() const;

    bool hasMoreOutput() const;

private:
    std::vector<uint8_t> mInput;
    std::vector<uint8_t> mOutput;
    size_t mOutputBytesHeadOffset;
};

} // namespace faketransport
} // namespace rt
