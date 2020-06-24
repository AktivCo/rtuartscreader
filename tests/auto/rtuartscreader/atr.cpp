#include <rtuartscreader/iso7816_3/atr.h>

#include <initializer_list>
#include <memory>

#include <gtest/gtest.h>

#include <rtuartscreader/utils/common.h>

#include <faketransport/simplecard.h>

#include "constants.h"

using namespace std;
using namespace testing;

namespace rtft = rt::faketransport;

namespace {

uint8_t randByte() {
    return static_cast<uint8_t>(rand() & 0xff);
}

} // namespace

bool operator==(const f_d_index_t& lhs, const f_d_index_t& rhs) {
    return memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}

class TestAtr : public testing::Test {
public:
    virtual void TearDown() override {
        rtft::resetCard();
    }

    void setupCardOutput(vector<uint8_t> cardOutput) {
        auto card = make_shared<rtft::SimpleCard>(move(cardOutput));
        rtft::setCard(card);
    }

    auto getAtr() {
        atr_t atr;
        auto r = read_atr(nullptr, &atr);
        if (r != iso7816_3_status_ok) throw runtime_error("read_atr failed");

        return atr;
    }

    auto parseAtr(const atr_t& atr) {
        atr_info_t atr_info;
        auto r = parse_atr(&atr, &atr_info);
        if (r != iso7816_3_status_ok) throw runtime_error("parse_atr failed");

        return atr_info;
    }
};

TEST_F(TestAtr, Rutoken2151) {
    /*
    0x3b, 0x1a, 0x96, 0x72, 0x75, 0x74, 0x6f, 0x6b,
    0x65, 0x6e, 0x6d, 0x73, 0x63
    */
    vector<uint8_t> cardOutput{ kAtr2151 };
    setupCardOutput(cardOutput);

    auto atr = getAtr();

    EXPECT_EQ(cardOutput, vector<uint8_t>(atr.atr, atr.atr + atr.atr_len));
    EXPECT_EQ(1, atr.t0_offset);
    EXPECT_EQ(2, atr.ta_offset[0]);
    for (size_t i = 1; i < ARRAYSIZE(atr.ta_offset); ++i) {
        EXPECT_EQ(BAD_ATR_OFFSET, atr.ta_offset[i]);
    }

    for (auto v : atr.tb_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }

    for (auto v : atr.tc_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }

    for (auto v : atr.td_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }


    EXPECT_EQ(3, atr.historical_bytes_offset);
    EXPECT_EQ(0x0a, atr.historical_bytes_len);

    EXPECT_EQ(BAD_ATR_OFFSET, atr.tck_offset);
}

TEST_F(TestAtr, ParseRutoken2151) {
    setupCardOutput(kAtr2151);

    auto atr_info = parseAtr(getAtr());

    EXPECT_TRUE(atr_info.ta1.is_present);
    EXPECT_EQ((f_d_index_t{ .f_index = 9, .d_index = 6 }), atr_info.ta1.f_d);

    EXPECT_FALSE(atr_info.tc1.is_present);
    EXPECT_FALSE(atr_info.ta2.is_present);
    EXPECT_FALSE(atr_info.tc2.is_present);

    for (const auto& protocol : atr_info.explicit_protocols) {
        EXPECT_FALSE(protocol);
    }
}

TEST_F(TestAtr, Rutoken2100T0) {
    /*
    0x3b, 0x9c, 0x96, 0x00, 0x52, 0x75, 0x74, 0x6f,
    0x6b, 0x65, 0x6e, 0x45, 0x43, 0x50, 0x73, 0x63
    */
    vector<uint8_t> cardOutput{ kAtr2100T0 };
    setupCardOutput(cardOutput);

    auto atr = getAtr();

    EXPECT_EQ(cardOutput, vector<uint8_t>(atr.atr, atr.atr + atr.atr_len));
    EXPECT_EQ(1, atr.t0_offset);
    EXPECT_EQ(2, atr.ta_offset[0]);
    EXPECT_EQ(3, atr.td_offset[0]);
    for (size_t i = 1; i < ARRAYSIZE(atr.ta_offset); ++i) {
        EXPECT_EQ(BAD_ATR_OFFSET, atr.ta_offset[i]);
    }

    for (auto v : atr.tb_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }

    for (auto v : atr.tc_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }

    for (size_t i = 1; i < ARRAYSIZE(atr.td_offset); ++i) {
        EXPECT_EQ(BAD_ATR_OFFSET, atr.td_offset[i]);
    }


    EXPECT_EQ(4, atr.historical_bytes_offset);
    EXPECT_EQ(0x0c, atr.historical_bytes_len);

    EXPECT_EQ(BAD_ATR_OFFSET, atr.tck_offset);
}

TEST_F(TestAtr, ParseRutoken2100T0) {
    setupCardOutput(kAtr2100T0);

    auto atr_info = parseAtr(getAtr());

    EXPECT_TRUE(atr_info.ta1.is_present);
    EXPECT_EQ((f_d_index_t{ .f_index = 9, .d_index = 6 }), atr_info.ta1.f_d);

    EXPECT_FALSE(atr_info.tc1.is_present);
    EXPECT_FALSE(atr_info.ta2.is_present);
    EXPECT_FALSE(atr_info.tc2.is_present);

    EXPECT_TRUE(atr_info.explicit_protocols[0]);
    for_each(begin(atr_info.explicit_protocols) + 1, end(atr_info.explicit_protocols), [](const auto& protocol) {
        EXPECT_FALSE(protocol);
    });
}

TEST_F(TestAtr, Rutoken2100T1) {
    /*
    0x3b, 0x9c, 0x94, 0x80, 0x11, 0x40, 0x52, 0x75,
    0x74, 0x6f, 0x6b, 0x65, 0x6e, 0x45, 0x43, 0x50,
    0x73, 0x63, 0xc3
    */
    vector<uint8_t> cardOutput{ kAtr2100T1 };
    setupCardOutput(cardOutput);

    auto atr = getAtr();

    EXPECT_EQ(cardOutput, vector<uint8_t>(atr.atr, atr.atr + atr.atr_len));
    EXPECT_EQ(1, atr.t0_offset);
    EXPECT_EQ(2, atr.ta_offset[0]);
    EXPECT_EQ(3, atr.td_offset[0]);
    EXPECT_EQ(4, atr.td_offset[1]);
    EXPECT_EQ(5, atr.ta_offset[2]);

    EXPECT_EQ(BAD_ATR_OFFSET, atr.ta_offset[1]);
    for (size_t i = 3; i < ARRAYSIZE(atr.ta_offset); ++i) {
        EXPECT_EQ(BAD_ATR_OFFSET, atr.ta_offset[i]);
    }

    for (auto v : atr.tb_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }

    for (auto v : atr.tc_offset) {
        EXPECT_EQ(BAD_ATR_OFFSET, v);
    }

    for (size_t i = 2; i < ARRAYSIZE(atr.td_offset); ++i) {
        EXPECT_EQ(BAD_ATR_OFFSET, atr.td_offset[i]);
    }


    EXPECT_EQ(6, atr.historical_bytes_offset);
    EXPECT_EQ(0x0c, atr.historical_bytes_len);

    EXPECT_EQ(cardOutput.size() - 1, atr.tck_offset);
}

TEST_F(TestAtr, ParseRutoken2100T1) {
    setupCardOutput(kAtr2100T1);

    auto atr_info = parseAtr(getAtr());

    EXPECT_TRUE(atr_info.ta1.is_present);
    EXPECT_EQ((f_d_index_t{ .f_index = 9, .d_index = 0x04 }), atr_info.ta1.f_d);

    EXPECT_FALSE(atr_info.tc1.is_present);
    EXPECT_FALSE(atr_info.ta2.is_present);
    EXPECT_FALSE(atr_info.tc2.is_present);

    EXPECT_TRUE(atr_info.explicit_protocols[0]);
    EXPECT_TRUE(atr_info.explicit_protocols[1]);
    for_each(begin(atr_info.explicit_protocols) + 2, end(atr_info.explicit_protocols), [](const auto& protocol) {
        EXPECT_FALSE(protocol);
    });
}

TEST_F(TestAtr, InvalidTs) {
    auto ts = randByte();
    if (ts == 0x3B) ++ts;

    vector<uint8_t> cardOutput;
    cardOutput.push_back(ts);

    setupCardOutput(move(cardOutput));

    atr_t atr;
    auto r = read_atr(nullptr, &atr);
    EXPECT_EQ(iso7816_3_status_unexpected_card_response, r);
}

TEST_F(TestAtr, InvalidTck) {
    vector<uint8_t> cardOutput{ kAtr2100T1 };
    ++cardOutput.back();

    setupCardOutput(move(cardOutput));

    atr_t atr;
    auto r = read_atr(nullptr, &atr);
    EXPECT_EQ(iso7816_3_status_unexpected_card_response, r);
}

TEST_F(TestAtr, MaxLength) {
    initializer_list<uint8_t> cardOutput{
        0x3b,
        0xff,
        0xff, 0xff, 0xff, 0xf0,
        0xff, 0xff, 0xff, 0xf0,
        0xff, 0xff, 0xff, 0xf0,
        0xff, 0xff, 0xff, 0x00,
        /* HISTORICAL */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };

    ASSERT_EQ(ARRAYSIZE(declval<atr_t>().atr), cardOutput.size());

    setupCardOutput(cardOutput);
    (void)getAtr();
}

class TestAtrOverflow : public TestAtr, public WithParamInterface<const initializer_list<uint8_t>*> {
};

TEST_P(TestAtrOverflow, Overflow) {
    ASSERT_EQ(ARRAYSIZE(declval<atr_t>().atr) + 1, GetParam()->size());

    setupCardOutput(*GetParam());

    atr_t atr;
    auto r = read_atr(nullptr, &atr);
    EXPECT_EQ(iso7816_3_status_unexpected_card_response, r);
}

namespace {
initializer_list<uint8_t> kAtrHistoricalOverflow{
    0x3b,
    0xff,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0x10,
    0xff,
    /* HISTORICAL */
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff
};

initializer_list<uint8_t> kAtrTaOverflow{
    0x3b,
    0x90,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xB0,
    0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0x10,
    0xff
};

initializer_list<uint8_t> kAtrTbOverflow{
    0x3b,
    0x90,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xB0,
    0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0x20,
    0xff
};

initializer_list<uint8_t> kAtrTcOverflow{
    0x3b,
    0x90,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xB0,
    0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0x40,
    0xff
};

initializer_list<uint8_t> kAtrTdOverflow{
    0x3b,
    0x90,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xB0,
    0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0x80,
    0x00
};

initializer_list<uint8_t> kAtrTckOverflow{
    0x3b,
    0xfe,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0xf0,
    0xff, 0xff, 0xff, 0x11,
    0xff,
    /* HISTORICAL */
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,
    /* TCK */
    0xff
};

} // namespace

INSTANTIATE_TEST_SUITE_P(Historical, TestAtrOverflow, Values(&kAtrHistoricalOverflow));
INSTANTIATE_TEST_SUITE_P(Ta, TestAtrOverflow, Values(&kAtrTaOverflow));
INSTANTIATE_TEST_SUITE_P(Tb, TestAtrOverflow, Values(&kAtrTbOverflow));
INSTANTIATE_TEST_SUITE_P(Tc, TestAtrOverflow, Values(&kAtrTcOverflow));
INSTANTIATE_TEST_SUITE_P(Td, TestAtrOverflow, Values(&kAtrTdOverflow));
INSTANTIATE_TEST_SUITE_P(Tck, TestAtrOverflow, Values(&kAtrTckOverflow));

namespace {
initializer_list<uint8_t> kTestAtr{
    0x3b,
    0xf0,
    0xff, 0xff, 0xff, 0x70,
    0xff, 0xff, 0xff
};
} // namespace

TEST_F(TestAtr, ParseTA1) {
    vector<uint8_t> cardOutput{ kTestAtr };

    initializer_list<uint8_t> values = { 0x01, 0xef };

    for (const auto& value : values) {
        cardOutput[2] = value;
        setupCardOutput(cardOutput);

        auto atr_info = parseAtr(getAtr());
        EXPECT_TRUE(atr_info.ta1.is_present);
        f_d_index_t f_d_index;
        f_d_index.f_index = value >> 4;
        f_d_index.d_index = value & 0x0f;
        EXPECT_EQ(f_d_index, atr_info.ta1.f_d);
    }
}

TEST_F(TestAtr, ParseTA2) {
    vector<uint8_t> cardOutput{ kTestAtr };

    initializer_list<uint8_t> values = { 0x00, 0xff };

    for (const auto& value : values) {
        cardOutput[6] = value;
        setupCardOutput(cardOutput);

        auto atr_info = parseAtr(getAtr());
        EXPECT_TRUE(atr_info.ta2.is_present);
        EXPECT_EQ(!(value & 0x80), atr_info.ta2.can_change_mode);
        EXPECT_EQ(!!(value & 0x10), atr_info.ta2.use_implicit_f_d);
        EXPECT_EQ((value & 0x0f), atr_info.ta2.enforced_protocol);
    }
}

TEST_F(TestAtr, ParseTC1) {
    vector<uint8_t> cardOutput{ kTestAtr };

    initializer_list<uint8_t> values = { 0x00, 0xff };

    for (const auto& value : values) {
        cardOutput[4] = value;
        setupCardOutput(cardOutput);

        auto atr_info = parseAtr(getAtr());
        EXPECT_TRUE(atr_info.tc1.is_present);
        EXPECT_EQ(value, atr_info.tc1.n);
    }
}

TEST_F(TestAtr, ParseTC2) {
    vector<uint8_t> cardOutput{ kTestAtr };

    initializer_list<uint8_t> values = { 0x00, 0xff };

    for (const auto& value : values) {
        cardOutput[8] = value;
        setupCardOutput(cardOutput);

        auto atr_info = parseAtr(getAtr());
        EXPECT_TRUE(atr_info.tc2.is_present);
        EXPECT_EQ(value, atr_info.tc2.wi);
    }
}

TEST_F(TestAtr, ParseInterfaceBytesAbsent) {
    initializer_list<uint8_t> cardOutput{ 0x3b, 0x01, 0xff };
    setupCardOutput(cardOutput);

    auto atr_info = parseAtr(getAtr());
    EXPECT_FALSE(atr_info.ta1.is_present);
    EXPECT_FALSE(atr_info.ta2.is_present);
    EXPECT_FALSE(atr_info.tc1.is_present);
    EXPECT_FALSE(atr_info.tc2.is_present);
}
