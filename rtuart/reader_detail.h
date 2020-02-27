#pragma once

#include <PCSC/ifdhandler.h>

#include <TransportAPI/TransportAPI.h>

typedef enum reader_power_state_enum {
    POWERED_OFF = 0,
    POWERED_ON
} POWER_STATE;

typedef struct reader_st {
    POWER_STATE power;
    DEVICE_HANDLE handle;
    UCHAR atr[MAX_ATR_SIZE];
    DWORD atrLength;
} Reader;