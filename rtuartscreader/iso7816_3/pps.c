// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/iso7816_3/pps.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <rtuartscreader/iso7816_3/detail/error.h>
#include <rtuartscreader/iso7816_3/detail/utils.h>
#include <rtuartscreader/transport/sendrecv.h>
#include <rtuartscreader/utils/common.h>

#define PPS1_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 1))
#define PPS2_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 2))
#define PPS3_IS_PRESENT(x) (!!NTH_BIT_ONLY(x, 3))

#define PPS_MAX_LENGTH 6

// All offset fields have values in between 0..PPS_MAX_LENGTH
// or BAD_ATR_OFFSET, if there is no such byte in PPS
typedef struct pps {
    uint8_t pps[PPS_MAX_LENGTH];
    size_t pps_len;
    uint8_t pps1_offset;
    uint8_t pps2_offset;
    uint8_t pps3_offset;
    uint8_t pck_offset;
} pps_t;

typedef struct pps1 {
    bool is_present;
    f_d_index_t f_d;
} pps1_t;

typedef struct pps2 {
    bool is_present;
    uint8_t spu;
} pps2_t;

typedef struct pps3 {
    bool is_present;
    uint8_t value;
} pps3_t;

typedef struct pps_info {
    uint8_t protocol;
    pps1_t pps1;
    pps2_t pps2;
    pps3_t pps3;
} pps_info_t;

typedef struct pps_headers {
    uint8_t pps1 : 1;
    uint8_t pps2 : 1;
    uint8_t pps3 : 1;
} pps_headers_t;

static void init_pps_headers(pps_headers_t* pps, uint8_t pps0) {
    uint8_t hioct = HIOCT(pps0);
    pps->pps1 = PPS1_IS_PRESENT(hioct);
    pps->pps2 = PPS2_IS_PRESENT(hioct);
    pps->pps3 = PPS3_IS_PRESENT(hioct);
}

static void parse_pps(const pps_t* pps, pps_info_t* pps_info) {
    memset(pps_info, 0, sizeof(*pps_info));

    pps_info->protocol = pps->pps[1] & 0x0F;

    pps_info->pps1.is_present = (pps->pps1_offset != BAD_ATR_OFFSET);
    if (pps_info->pps1.is_present) {
        uint8_t pps1_value = pps->pps[pps->pps1_offset];
        pps_info->pps1.f_d.f_index = HIOCT(pps1_value);
        pps_info->pps1.f_d.d_index = LOWOCT(pps1_value);
    }

    pps_info->pps2.is_present = (pps->pps2_offset != BAD_ATR_OFFSET);
    if (pps_info->pps2.is_present) {
        pps_info->pps2.spu = pps->pps[pps->pps2_offset];
    }

    pps_info->pps3.is_present = (pps->pps3_offset != BAD_ATR_OFFSET);
    if (pps_info->pps3.is_present) {
        pps_info->pps3.value = pps->pps[pps->pps3_offset];
    }
}

static void build_pps_request(const f_d_index_t* f_d_index, uint8_t protocol, pps_t* pps) {
    memset(pps, BAD_ATR_OFFSET, sizeof(*pps));

    pps->pps_len = 4;
    pps->pps1_offset = 2;
    pps->pck_offset = 3;

    pps->pps[0] = 0xFF;
    pps->pps[1] = 0x10 | (protocol & 0x0F);
    pps->pps[pps->pps1_offset] = (f_d_index->f_index << 4) | f_d_index->d_index;

    uint8_t pck = 0;
    for (size_t i = 0; i < pps->pps_len - 1; ++i) {
        pck ^= pps->pps[i];
    }
    pps->pps[pps->pck_offset] = pck;
}

static iso7816_3_status_t read_pps_response(const transport_t* transport, pps_t* pps) {
    size_t i = 0;
    memset(pps, BAD_ATR_OFFSET, sizeof(*pps));

    uint8_t ppss;
    transport_status_t r = transport_recv_byte(transport, &ppss);
    RETURN_ON_TRANSPORT_ERROR(r);
    pps->pps[i++] = ppss;

    if (ppss != 0xFF)
        LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_unexpected_card_response, "Bad PPSS");

    uint8_t pps0;
    r = transport_recv_byte(transport, &pps0);
    RETURN_ON_TRANSPORT_ERROR(r);
    pps->pps[i++] = pps0;

    pps_headers_t ppsi;
    init_pps_headers(&ppsi, pps0);

    if (ppsi.pps1) {
        uint8_t pps1;
        r = transport_recv_byte(transport, &pps1);
        RETURN_ON_TRANSPORT_ERROR(r);
        pps->pps1_offset = i;
        pps->pps[i++] = pps1;
    }

    if (ppsi.pps2) {
        uint8_t pps2;
        r = transport_recv_byte(transport, &pps2);
        RETURN_ON_TRANSPORT_ERROR(r);
        pps->pps2_offset = i;
        pps->pps[i++] = pps2;
    }

    if (ppsi.pps3) {
        uint8_t pps3;
        r = transport_recv_byte(transport, &pps3);
        RETURN_ON_TRANSPORT_ERROR(r);
        pps->pps3_offset = i;
        pps->pps[i++] = pps3;
    }

    uint8_t pck;
    r = transport_recv_byte(transport, &pck);
    RETURN_ON_TRANSPORT_ERROR(r);
    pps->pck_offset = i;
    pps->pps[i++] = pck;

    pps->pps_len = i;

    uint8_t convolution = 0;
    for (size_t i = 0; i < pps->pps_len; ++i) {
        convolution ^= pps->pps[i];
    }
    if (convolution != 0)
        LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_unexpected_card_response, "Incorrect PCK");

    return iso7816_3_status_ok;
}

static bool is_pps_exchange_success(const pps_t* pps_request, const pps_t* pps_response) {
    pps_info_t pps_info_request;
    parse_pps(pps_request, &pps_info_request);

    pps_info_t pps_info_response;
    parse_pps(pps_response, &pps_info_response);

    if (pps_info_request.protocol != pps_info_response.protocol) {
        return false;
    }

    if (pps_info_request.pps1.is_present) {
        if (pps_info_response.pps1.is_present //
            && memcmp(&pps_info_request.pps1.f_d, &pps_info_response.pps1.f_d, sizeof(pps_info_request.pps1.f_d))) {
            return false;
        }

        if (!pps_info_response.pps1.is_present //
            && memcmp(&pps_info_request.pps1.f_d, &f_d_index_default, sizeof(pps_info_request.pps1.f_d))) {
            return false;
        }
    } else if (pps_info_response.pps1.is_present) {
        return false;
    }

    if (pps_info_request.pps2.is_present) {
        if (pps_info_response.pps2.is_present && pps_info_request.pps2.spu != pps_info_response.pps2.spu) {
            return false;
        }

        if (!pps_info_response.pps2.is_present && pps_info_request.pps2.spu != 0) {
            return false;
        }
    } else if (pps_info_response.pps2.is_present) {
        return false;
    }

    if (pps_info_request.pps3.is_present) {
        if (pps_info_response.pps3.is_present && pps_info_request.pps3.value != pps_info_response.pps3.value) {
            return false;
        }

        if (!pps_info_response.pps3.is_present && pps_info_request.pps3.value != 0) {
            return false;
        }
    } else if (pps_info_response.pps3.is_present) {
        return false;
    }

    return true;
}

iso7816_3_status_t do_pps_exchange(const transport_t* transport, const f_d_index_t* f_d_index, uint8_t protocol) {
    pps_t pps_request;
    build_pps_request(f_d_index, protocol, &pps_request);

    transport_status_t r = transport_send_bytes(transport, pps_request.pps, pps_request.pps_len);
    RETURN_ON_TRANSPORT_ERROR(r);

    pps_t pps_response;
    iso7816_3_status_t atr_r = read_pps_response(transport, &pps_response);
    if (atr_r != iso7816_3_status_ok)
        return atr_r;

    if (!is_pps_exchange_success(&pps_request, &pps_response)) {
        LOG_RETURN_ISO7816_3_ERROR(iso7816_3_status_pps_exchange_failed);
    }

    return iso7816_3_status_ok;
}