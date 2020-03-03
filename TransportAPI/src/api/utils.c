// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "utils.h"

#include <string.h>

#define LOBYTE(w) ((uint8_t)(w))
#define HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#define LOWORD(w) ((uint16_t)(w))
#define HIWORD(w) ((uint16_t)(((uint32_t)(w) >> 16) & 0xFFFF))

void pack_header(PCTRANSPORT_PACKET_HEADER pHdr, uint8_t *hdrData) {
    hdrData[0] = pHdr->bHdrSize;
    hdrData[1] = pHdr->bHdrVers;
    hdrData[2] = pHdr->bProtocolVers;
    hdrData[3] = pHdr->bPID;
    hdrData[4] = LOBYTE(LOWORD(pHdr->dwDataCrc32));
    hdrData[5] = HIBYTE(LOWORD(pHdr->dwDataCrc32));
    hdrData[6] = LOBYTE(HIWORD(pHdr->dwDataCrc32));
    hdrData[7] = HIBYTE(HIWORD(pHdr->dwDataCrc32));
    hdrData[8] = LOBYTE(pHdr->wPacketNum);
    hdrData[9] = HIBYTE(pHdr->wPacketNum);
    hdrData[10] = LOBYTE(pHdr->wSize);
    hdrData[11] = HIBYTE(pHdr->wSize);
    hdrData[12] = pHdr->bRet;
    hdrData[13] = pHdr->abRes[0];
    hdrData[14] = pHdr->abRes[1];
    hdrData[15] = pHdr->abRes[2];
}

void unpack_header(PTRANSPORT_PACKET_HEADER pHdr, const uint8_t *hdrData) {
    pHdr->bHdrSize = hdrData[0];
    pHdr->bHdrVers = hdrData[1];
    pHdr->bProtocolVers = hdrData[2];
    pHdr->bPID = hdrData[3];
    pHdr->dwDataCrc32 = (uint32_t)(hdrData[4]) + ((uint32_t)(hdrData[5]) << 8) +
                        ((uint32_t)(hdrData[6]) << 16) + ((uint32_t)(hdrData[7]) << 24);
    pHdr->wPacketNum = (uint16_t)(hdrData[8]) + ((uint16_t)(hdrData[9]) << 8);
    pHdr->wSize = (uint16_t)(hdrData[10]) + ((uint16_t)(hdrData[11]) << 8);
    pHdr->bRet = hdrData[12];
    pHdr->abRes[0] = hdrData[13];
    pHdr->abRes[1] = hdrData[14];
    pHdr->abRes[2] = hdrData[15];
}

void pack_header_null_crc(PCTRANSPORT_PACKET_HEADER pHdr, uint8_t *hdrData) {
    pack_header(pHdr, hdrData);

    memset(hdrData + 4, 0, 4);
}
