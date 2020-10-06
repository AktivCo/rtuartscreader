// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/iso7816_3/pps.h>

#include <initializer_list>
#include <memory>
#include <numeric>

#include <gtest/gtest.h>

#include <rtuartscreader/utils/common.h>

#include <faketransport/simplecard.h>

using namespace std;
using namespace testing;

namespace rtft = rt::faketransport;

class TestPps : public Test {
public:
    virtual void TearDown() override {
        rtft::resetCard();
    }

    void setupCardOutput(vector<uint8_t> cardOutput) {
        mCard = make_shared<rtft::SimpleCard>(move(cardOutput));
        rtft::setCard(mCard);
    }

protected:
    shared_ptr<rtft::SimpleCard> mCard;
};

TEST_F(TestPps, BuildRequest) {
    setupCardOutput({});

    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 1;

    EXPECT_EQ(iso7816_3_status_communication_error, do_pps_exchange(nullptr, &f_d_index, protocol));

    const auto& input = mCard->getInput();

    EXPECT_LE(4, input.size());
    EXPECT_GE(6, input.size());

    EXPECT_EQ(0xFF, input[0]);

    EXPECT_EQ(protocol, input[1] & 0x0f);

    auto ppsiMask = input[1] >> 4;

    EXPECT_TRUE(ppsiMask & 0x01);
    EXPECT_EQ((f_d_index.f_index << 4) | f_d_index.d_index, input[2]);

    if (ppsiMask & 0x02) {
        EXPECT_LE(5, input.size());
        EXPECT_EQ(0, input[3]);
    }

    if (ppsiMask & 0x04) {
        EXPECT_EQ(6, input.size());
        EXPECT_EQ(0, input[4]);
    }

    auto check = accumulate(input.begin(), input.end(), 0, [](uint8_t l, uint8_t r) { return l ^ r; });
    EXPECT_EQ(0, check);
}


TEST_F(TestPps, ReadPpsIncomplete) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 1;

    vector<uint8_t> cardOutput = { 0xFF, 0x71, 0xa5, 0x00, 0x00 };

    for (auto it = cardOutput.begin(); it < cardOutput.end(); ++it) {
        vector<uint8_t> testedCardOutput{ cardOutput.begin(), it };
        setupCardOutput(testedCardOutput);
        EXPECT_EQ(iso7816_3_status_communication_error, do_pps_exchange(nullptr, &f_d_index, protocol));
    }
}

TEST_F(TestPps, ReadPpsWrongPpck) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 1;

    vector<uint8_t> cardOutput = { 0xFF, 0x71, 0xa5, 0x00, 0x00, 0x00 };
    setupCardOutput(cardOutput);
    EXPECT_EQ(iso7816_3_status_unexpected_card_response, do_pps_exchange(nullptr, &f_d_index, protocol));
}

namespace {
uint8_t makePps1(const f_d_index_t& f_d_index) {
    return static_cast<uint8_t>(f_d_index.f_index << 4 | f_d_index.d_index);
}

template<typename T>
uint8_t countPck(const T& cardOutput) {
    return accumulate(cardOutput.begin(), cardOutput.end() - 1, 0, [](uint8_t l, uint8_t r) { return l ^ r; });
}
} // namespace

TEST_F(TestPps, MatchPpsSuccess) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = { 0xFF, static_cast<uint8_t>(0x10 | protocol), makePps1(f_d_index), 0x00 };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_ok, do_pps_exchange(nullptr, &f_d_index, protocol));
}

TEST_F(TestPps, MatchPpsWrongProtocol) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = { 0xFF, static_cast<uint8_t>(0x10 | (protocol ^ 0x0f)), makePps1(f_d_index), 0x00 };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_pps_exchange_failed, do_pps_exchange(nullptr, &f_d_index, protocol));
}

TEST_F(TestPps, MatchPpsWrongFIndex) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = {
        0xFF, static_cast<uint8_t>(0x10 | protocol),
        makePps1(f_d_index_t{ static_cast<uint8_t>(f_d_index.f_index ^ 0xFF), f_d_index.d_index }), 0x00
    };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_pps_exchange_failed, do_pps_exchange(nullptr, &f_d_index, protocol));
}

TEST_F(TestPps, MatchPpsWrongDIndex) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = {
        0xFF, static_cast<uint8_t>(0x10 | protocol),
        makePps1(f_d_index_t{ f_d_index.f_index, static_cast<uint8_t>(f_d_index.d_index ^ 0xFF) }), 0x00
    };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_pps_exchange_failed, do_pps_exchange(nullptr, &f_d_index, protocol));
}

// This test is implementation-defined. It does not cover all cases defined in 9.3 iso7816-3
// because we make specific PPS Request without PPS2/PPS3.
TEST_F(TestPps, MatchPpsResponseCantHavePps2) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = { 0xFF, static_cast<uint8_t>(0x30 | protocol), makePps1(f_d_index), 0x00, 0x00 };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_pps_exchange_failed, do_pps_exchange(nullptr, &f_d_index, protocol));
}

// This test is implementation-defined. It does not cover all cases defined in 9.3 iso7816-3
// because we make specific PPS Request without PPS2/PPS3.
TEST_F(TestPps, MatchPpsResponseCantHavePps3) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = { 0xFF, static_cast<uint8_t>(0x50 | protocol), makePps1(f_d_index), 0x00, 0x00 };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_pps_exchange_failed, do_pps_exchange(nullptr, &f_d_index, protocol));
}

TEST_F(TestPps, MatchPpsResponseCanHaveEmptyPps1) {
    f_d_index_t f_d_index = { 0x0a, 0x05 };
    uint8_t protocol = 0x0f;

    vector<uint8_t> cardOutput = { 0xFF, static_cast<uint8_t>(0x00 | protocol), 0x00 };
    cardOutput.back() = countPck(cardOutput);

    setupCardOutput(cardOutput);

    EXPECT_EQ(iso7816_3_status_pps_exchange_use_default_f_d, do_pps_exchange(nullptr, &f_d_index, protocol));
}
