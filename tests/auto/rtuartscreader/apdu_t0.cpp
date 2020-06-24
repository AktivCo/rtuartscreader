#include <rtuartscreader/apdu_t0.h>

#include <memory>

#include <gtest/gtest.h>

#include <faketransport/faketransport.h>
#include <faketransport/simplecard.h>

using namespace std;

namespace rtft = rt::faketransport;

class TestT0 : public testing::Test {
    virtual void TearDown() override {
        rtft::resetCard();
    }
};

TEST_F(TestT0, Sample) {
    vector<uint8_t> cardOutput = { 0x90, 0x00 };
    auto card = make_shared<rtft::SimpleCard>(cardOutput);

    rtft::setCard(card);

    vector<uint8_t> apdu{ 0x80, 0x00, 0x00, 0x00 };
    vector<uint8_t> response(257);
    uint16_t responseLength = response.size();

    t0_transmit_apdu(nullptr, apdu.data(), apdu.size(), response.data(), &responseLength);
    response.resize(responseLength);
    EXPECT_EQ(cardOutput, response);
}
