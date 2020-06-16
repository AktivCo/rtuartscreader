#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <PCSC/ifdhandler.h>

#include <rtuartscreader/transport/transport_t.h>

#define PROTOCOL_T0 0
#define MAX_PROTOCOL_VALUE 15

typedef enum {
    atr_status_ok = 0,
    atr_status_communication_error,
    atr_status_invalid_atr
} atr_status_t;

#define MAX_INTERFACE_BYTES_COUNT (MAX_ATR_SIZE - 3) // Anything except T0 & TCK

#define BAD_ATR_OFFSET 0xFF

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

typedef struct f_d_index {
    uint8_t f_index;
    uint8_t d_index;
} f_d_index_t;

extern const f_d_index_t f_d_index_default;

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

atr_status_t read_atr(const transport_t* transport, atr_t* info);

atr_status_t parse_atr(const atr_t* atr, atr_info_t* info);
