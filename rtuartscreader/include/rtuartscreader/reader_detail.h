// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <PCSC/ifdhandler.h>

#include <rtuartscreader/transport/transport_t.h>

typedef enum reader_power_state_enum {
    POWERED_OFF = 0,
    POWERED_ON
} POWER_STATE;

typedef enum reader_card_presence_enum {
    PRESENT_FALSE = 0,
    PRESENT_TRUE
} CARD_PRESENCE;

typedef struct reader_st {
    POWER_STATE power;
    CARD_PRESENCE presence;
    UCHAR atr[MAX_ATR_SIZE];
    DWORD atrLength;
    transport_t transport;
} Reader;
