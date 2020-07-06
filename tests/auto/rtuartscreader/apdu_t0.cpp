// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/iso7816_3/apdu_t0.h>

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
