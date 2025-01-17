// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <PCSC/ifdhandler.h>

#include <rtuartscreader/iso7816_3/detail/utils.h>
#include <rtuartscreader/iso7816_3/f_d_index.h>
#include <rtuartscreader/iso7816_3/status.h>
#include <rtuartscreader/transport/transport_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROTOCOL_T0 0
#define PROTOCOL_T1 1
#define MAX_PROTOCOL_VALUE 15

#define MAX_INTERFACE_BYTES_COUNT (MAX_ATR_SIZE - 3) // Anything except T0 & TCK

// All offset fields have values in between 0..MAX_ATR_SIZE
// or BAD_ATR_OFFSET, if there is no such byte in ATR
typedef struct atr {
    uint8_t atr[MAX_ATR_SIZE];
    size_t atr_len;
    uint8_t t0_offset;
    uint8_t ta_offset[MAX_INTERFACE_BYTES_COUNT];
    uint8_t tb_offset[MAX_INTERFACE_BYTES_COUNT];
    uint8_t tc_offset[MAX_INTERFACE_BYTES_COUNT];
    uint8_t td_offset[MAX_INTERFACE_BYTES_COUNT];
    uint8_t historical_bytes_offset;
    size_t historical_bytes_len;
    uint8_t tck_offset;
} atr_t;

typedef struct ta1 {
    bool is_present;
    f_d_index_t f_d;
} ta1_t;

typedef struct ta2 {
    bool is_present;
    bool can_change_mode;
    bool use_implicit_f_d;
    uint8_t enforced_protocol;
} ta2_t;

typedef struct tc1 {
    bool is_present;
    uint8_t n;
} tc1_t;

typedef struct tc2 {
    bool is_present;
    uint8_t wi;
} tc2_t;

typedef struct atr_info {
    ta1_t ta1;
    tc1_t tc1;
    ta2_t ta2;
    tc2_t tc2;

    bool explicit_protocols[MAX_PROTOCOL_VALUE + 1];
} atr_info_t;

iso7816_3_status_t read_atr(const transport_t* transport, atr_t* info);

iso7816_3_status_t parse_atr(const atr_t* atr, atr_info_t* info);

#ifdef __cplusplus
}
#endif
