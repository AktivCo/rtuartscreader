#include <faketransport/simplecard.h>

#include <algorithm>
#include <stdexcept>

using namespace std;

namespace rt {
namespace faketransport {

void SimpleCard::input(const uint8_t* buffer, size_t length) {
    mInput.insert(mInput.end(), buffer, buffer + length);
}

void SimpleCard::output(uint8_t* buffer, size_t length) {
    if (mOutputBytesHeadOffset + length > mOutput.size()) {
        throw std::runtime_error("Not enough data in output");
    }

    copy(mOutput.begin() + mOutputBytesHeadOffset, mOutput.begin() + mOutputBytesHeadOffset + length, buffer);
    mOutputBytesHeadOffset += length;
}

const vector<uint8_t>& SimpleCard::getInput() const {
    return mInput;
}

vector<uint8_t> SimpleCard::getTransmittedOutput() const {
    return vector<uint8_t>{ mOutput.begin(), mOutput.begin() + mOutputBytesHeadOffset };
}

bool SimpleCard::hasMoreOutput() const {
    return mOutput.size() != mOutputBytesHeadOffset;
}

} // namespace faketransport
} // namespace rt
