// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/transport/reset.h>

#include <algorithm>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <rtuartscreader/transport/detail/transmit_params.h>

#include <faketransport/initialize.h>
#include <faketransport/simplecard.h>

#include "constants.h"
#include "utils.h"

using namespace std;
using namespace ::testing;

namespace rtft = rt::faketransport;

class MockInitialize : public rtft::Initialize {
public:
    transport_status_t transport_reinitialize(transport_t* transport, const transmit_params_t* params) {
        auto r = do_transport_reinitialize(transport, params);
        if (r == transport_status_ok) {
            transport->params = *params;
        }
        return r;
    }

    MOCK_METHOD(transport_status_t, transport_initialize, (transport_t * transport, const char* name), (override));
    MOCK_METHOD(transport_status_t, transport_deinitialize, (const transport_t* transport), (override));

    MOCK_METHOD(transport_status_t, do_transport_reinitialize, (transport_t * transport, const transmit_params_t* params));
};

class EchoCard : public rtft::Card {
public:
    void input(const uint8_t* buffer, size_t length) override {
        mBuffer.insert(mBuffer.end(), buffer, buffer + length);
    }

    void output(uint8_t* buffer, size_t length) override {
        if (mOffset + length > mBuffer.size()) {
            throw runtime_error("Not enough data to output");
        }

        copy(mBuffer.begin() + mOffset, mBuffer.begin() + mOffset + length, buffer);
        mOffset += length;
    }

    const vector<uint8_t>& dataReceived() const {
        return mBuffer;
    }

    size_t dataSentCount() {
        return mOffset;
    }

private:
    vector<uint8_t> mBuffer = {};
    size_t mOffset = 0;
};

class EchoOnceCard : public EchoCard {
public:
    void input(const uint8_t* buffer, size_t length) override {
        if (!mIsReceiving) {
            throw runtime_error("Unexpected call sequence");
        }
        EchoCard::input(buffer, length);
    }

    void output(uint8_t* buffer, size_t length) override {
        mIsReceiving = false;
        EchoCard::output(buffer, length);
    }

private:
    bool mIsReceiving = true;
};

class OutputOnlyCard : public rtft::SimpleCard {
public:
    OutputOnlyCard(vector<uint8_t> buffer)
        : rtft::SimpleCard{ move(buffer) } {}

    void input(const uint8_t* buffer, size_t length) override {
        throw runtime_error("No input expected");
    }
};

class ResetCard : public rtft::Card {
public:
    ResetCard(vector<uint8_t> atr)
        : mCard{ make_unique<OutputOnlyCard>(move(atr)) } {
        switchCardIfAtrIsSent();
    }

    void input(const uint8_t* buffer, size_t length) override {
        mCard->input(buffer, length);
    }

    void output(uint8_t* buffer, size_t length) override {
        mCard->output(buffer, length);
        switchCardIfAtrIsSent();
    }

    vector<uint8_t> ppsRequest() const {
        auto card = dynamic_cast<EchoOnceCard*>(mCard.get());
        if (!card) {
            throw runtime_error("No pps");
        }

        return card->dataReceived();
    }

    vector<uint8_t> ppsResponse() const {
        auto card = dynamic_cast<EchoOnceCard*>(mCard.get());
        if (!card) {
            throw runtime_error("No pps");
        }

        vector<uint8_t> response{ card->dataReceived() };
        response.resize(card->dataSentCount());
        return response;
    }

private:
    void switchCardIfAtrIsSent() {
        if (auto card = dynamic_cast<OutputOnlyCard*>(mCard.get())) {
            if (!card->hasMoreOutput()) {
                mCard = make_unique<EchoOnceCard>();
            }
        }
    }

    unique_ptr<rtft::Card> mCard;
};

MATCHER(IsDefaultTransmitParams, "") {
    return 0 == memcmp(arg, transmit_params_default(), sizeof(transmit_params));
}

class TestResetRealAtr : public TestWithParam<const initializer_list<uint8_t>*> {
public:
    void SetUp() override {
        mTransport.params = *transmit_params_default();

        auto transportInitialize = make_unique<MockInitialize>();

        Sequence reinitializeSequence;
        EXPECT_CALL(*transportInitialize, do_transport_reinitialize(&mTransport, IsDefaultTransmitParams()))
            .InSequence(reinitializeSequence)
            .WillOnce(Return(transport_status_ok));
        EXPECT_CALL(*transportInitialize, do_transport_reinitialize(&mTransport, _))
            .InSequence(reinitializeSequence)
            .WillOnce(Return(transport_status_ok));

        rtft::setInitialize(move(transportInitialize));
    }
    void TearDown() override {
        rtft::resetCard();
        rtft::resetInitialize();
    }

protected:
    transport_t mTransport;
};

TEST_P(TestResetRealAtr, Positive) {
    const auto& kAtr = *GetParam();
    auto card = make_shared<ResetCard>(kAtr);
    rtft::setCard(card);

    vector<uint8_t> resultAtr(255);
    size_t atrLength = resultAtr.size();
    auto r = transport_reset(&mTransport, resultAtr.data(), &atrLength);
    EXPECT_EQ(transport_status_ok, r);
    resultAtr.resize(atrLength);

    EXPECT_EQ(card->ppsRequest(), card->ppsResponse()); // this means ppsResponse is fully read
    EXPECT_EQ(vector<uint8_t>{ kAtr }, resultAtr);

    EXPECT_LT(transmit_params_default()->transmit_speed.baudrate, mTransport.params.transmit_speed.baudrate);

    cout << "ATR:" << endl;
    printBuffer(cout, resultAtr);
    cout << endl;
    cout << "PPS Request:" << endl;
    printBuffer(cout, card->ppsRequest());
    cout << endl;

    cout << "Transmit params:" << endl
         << mTransport.params;
}

INSTANTIATE_TEST_SUITE_P(Rutoken2151, TestResetRealAtr, Values(&kAtr2151));
INSTANTIATE_TEST_SUITE_P(Rutoken2100T0, TestResetRealAtr, Values(&kAtr2100T0));
INSTANTIATE_TEST_SUITE_P(Rutoken2100T1, TestResetRealAtr, Values(&kAtr2100T1));
