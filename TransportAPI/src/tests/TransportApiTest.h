// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <TransportAPI/TransportAPI.h>

#define TRUE 1
#define FALSE 0

typedef enum {
    ST_NoErr = 0x00,
    ST_HdrSize = 0x01,      //Wrong header size
    ST_HdrVers = 0x02,      //Wrong header version
    ST_ProtocolVers = 0x02, //Wrong protocol version
    ST_PID = 0x04,          //Wrong packet type
    ST_PacketNum = 0x08,    //Wrong packet number
    ST_Size = 0x10,         //Wrong data size
    ST_DataCrc = 0x20,      //Wrong CRC computation
    ST_NoStart = 0x40,      //Absent initial control character ':'
    ST_NoEnd = 0x80,        //Absent last control character '\n'
    ST_NoEscape = 0x100,    //Absent encoding of escape characters '\n', ':', '$'
} SetTestError_t;


uint32_t
TrAPI_Test_Transmit(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufSend,
    uint16_t dwLngBufSend,
    uint8_t *pBufRecv,
    uint16_t *pdwLngBufRecv,
    SetTestError_t Error = ST_NoErr);
