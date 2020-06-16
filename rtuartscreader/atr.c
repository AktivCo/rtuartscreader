#include <rtuartscreader/atr.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <rtuartscreader/transport/sendrecv.h>
#include <rtuartscreader/utils/common.h>
#include <rtuartscreader/utils/error.h>

#define NTH_BIT_ONLY(x, bit_number) ((x) & (1ul << (bit_number - 1)))

#define TA_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 1))
#define TB_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 2))
#define TC_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 3))
#define TD_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 4))

#define HIOCT(x) ((x & 0xf0) >> 4)
#define LOWOCT(x) (x & 0x0f)

#define RETURN_ON_TRANSPORT_ERROR(r) POPULATE_ERROR(r, transport_status_ok, atr_status_communication_error)

#define SAFE_INCREMENT_N(counter, n, limit, error) \
    do {                                           \
        counter += n;                              \
        if (counter >= limit) {                    \
            return error;                          \
        }                                          \
    } while (0)

#define SAFE_INCREMENT(counter, limit, error) SAFE_INCREMENT_N(counter, 1, limit, error)

const f_d_index_t f_d_index_default = { .f_index = 1, .d_index = 1 };

typedef struct atr_headers {
    uint8_t ta : 2;
    uint8_t tb : 2;
    uint8_t tc : 2;
    uint8_t td : 2;
} atr_headers_t;

static void init_atr_headers(atr_headers_t* level, uint8_t td) {
    uint8_t hioct = HIOCT(td);
    level->ta = TA_IS_PRESENT(hioct);
    level->tb = TB_IS_PRESENT(hioct);
    level->tc = TC_IS_PRESENT(hioct);
    level->td = TD_IS_PRESENT(hioct);
}

static bool is_tck_present(const atr_t* atr) {
    // If only T=0 is indicated, possibly by default, then TCK shall be absent.
    // If T=0 and T=15 are present and in all the other cases, TCK shall be present.

    // T is encoded by low octet of TD_i. T=0 is default.
    // If TD1, TD2 and so on are present, the encoded types T shall be in ascending numerical order.
    for (size_t i = 0; atr->td_offset[i] != BAD_ATR_OFFSET; ++i) {
        size_t td_offset = atr->td_offset[i];

        if (LOWOCT(atr->atr[td_offset]) != PROTOCOL_T0)
            return true;
    }

    return false;
}

static bool is_tck_correct(const uint8_t* atr, size_t atr_len) {
    uint8_t convolution = 0;
    for (size_t i = 1; i < atr_len; ++i)
        convolution ^= atr[i];

    return convolution == 0;
}

static void init_atr(atr_t* atr) {
    memset(atr, BAD_ATR_OFFSET, sizeof(*atr));
    atr->atr_len = 0;
    atr->historical_bytes_len = 0;
}

atr_status_t read_atr(const transport_t* transport, atr_t* atr) {
    init_atr(atr);

    size_t i = 0;

    uint8_t ts;
    transport_status_t r = transport_recv_byte(transport, &ts);
    RETURN_ON_TRANSPORT_ERROR(r);
    atr->atr[i] = ts;

    if (ts != 0x3B)
        return atr_status_invalid_atr;

    uint8_t t0;
    r = transport_recv_byte(transport, &t0);
    RETURN_ON_TRANSPORT_ERROR(r);
    SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
    atr->t0_offset = i;
    atr->atr[i] = t0;

    atr_headers_t level_mask;
    init_atr_headers(&level_mask, t0);

    for (size_t level = 0;; ++level) {
        if (level_mask.ta) {
            uint8_t ta;
            r = transport_recv_byte(transport, &ta);
            RETURN_ON_TRANSPORT_ERROR(r);
            SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
            atr->ta_offset[level] = i;
            atr->atr[i] = ta;
        }

        if (level_mask.tb) {
            //TB is recommended to ingnore
            uint8_t tb;
            r = transport_recv_byte(transport, &tb);
            RETURN_ON_TRANSPORT_ERROR(r);
            SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
            atr->tb_offset[level] = i;
            atr->atr[i] = tb;
        }

        if (level_mask.tc) {
            uint8_t tc;
            r = transport_recv_byte(transport, &tc);
            RETURN_ON_TRANSPORT_ERROR(r);
            SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
            atr->tc_offset[level] = i;
            atr->atr[i] = tc;
        }

        if (level_mask.td) {
            uint8_t td;
            r = transport_recv_byte(transport, &td);
            RETURN_ON_TRANSPORT_ERROR(r);
            SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
            atr->td_offset[level] = i;
            atr->atr[i] = td;

            init_atr_headers(&level_mask, HIOCT(td));
        } else {
            break;
        }
    }

    atr->historical_bytes_len = LOWOCT(atr->atr[atr->t0_offset]);

    if (atr->historical_bytes_len) {
        SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
        atr->historical_bytes_offset = i;

        SAFE_INCREMENT_N(i, atr->historical_bytes_len - 1, MAX_ATR_SIZE, atr_status_invalid_atr);
        r = transport_recv_bytes(transport, atr->atr + atr->historical_bytes_offset, atr->historical_bytes_len);
        RETURN_ON_TRANSPORT_ERROR(r);
    }

    if (is_tck_present(atr)) {
        uint8_t tck;
        r = transport_recv_byte(transport, &tck);
        RETURN_ON_TRANSPORT_ERROR(r);
        SAFE_INCREMENT(i, MAX_ATR_SIZE, atr_status_invalid_atr);
        atr->tck_offset = i;
        atr->atr[i] = tck;

        atr->atr_len = i + 1;

        if (!is_tck_correct(atr->atr, atr->atr_len))
            return atr_status_invalid_atr;
    } else {
        atr->atr_len = i + 1;
    }

    return atr_status_ok;
}

static void init_atr_info(atr_info_t* info) {
    memset(info, 0, sizeof(*info));
}

atr_status_t parse_atr(const atr_t* atr, atr_info_t* info) {
    init_atr_info(info);

    if (atr->ta_offset[0] != BAD_ATR_OFFSET) {
        uint8_t ta1 = atr->atr[atr->ta_offset[0]];

        info->ta1.is_present = true;
        info->ta1.f_d.f_index = HIOCT(ta1);
        info->ta1.f_d.d_index = LOWOCT(ta1);
    }

    if (atr->ta_offset[1] != BAD_ATR_OFFSET) {
        uint8_t ta2 = atr->atr[atr->ta_offset[1]];

        info->ta2.is_present = true;
        info->ta2.can_change_mode = !NTH_BIT_ONLY(ta2, 8);
        info->ta2.use_implicit_f_d = !!NTH_BIT_ONLY(ta2, 5);
        info->ta2.enforced_protocol = LOWOCT(ta2);
    }

    if (atr->tc_offset[0] != BAD_ATR_OFFSET) {
        uint8_t tc1 = atr->atr[atr->tc_offset[0]];

        info->tc1.is_present = true;
        info->tc1.n = tc1;
    }

    if (atr->tc_offset[1] != BAD_ATR_OFFSET) {
        uint8_t tc2 = atr->atr[atr->tc_offset[1]];

        info->tc2.is_present = true;
        info->tc2.wi = tc2;
    }

    for (size_t i = 0; atr->td_offset[i] != BAD_ATR_OFFSET; ++i) {
        uint8_t tdi = atr->atr[atr->td_offset[i]];
        uint8_t protocol = LOWOCT(tdi);

        info->explicit_protocols[protocol] = true;
    }

    return atr_status_ok;
}
