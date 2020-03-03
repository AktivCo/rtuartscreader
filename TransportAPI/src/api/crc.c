// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "crc.h"

/* begin crc.c */
#define W_GX 0x8411
#define DW_GX 0xdebb20e3

uint32_t ComputeCRC32_Byte(uint32_t dwCRC, uint8_t bCalcByte) {
    uint8_t bI;

    for (bI = 0; bI < 8; bI++) {
        if (dwCRC & 0x80000000)
            dwCRC ^= DW_GX;
        dwCRC <<= 1;
        if (bCalcByte & 0x80)
            dwCRC++;
        bCalcByte <<= 1;
    }

    return (dwCRC);
}

uint32_t ComputeCRC32(uint32_t dwCRC, const uint8_t *pbBuf, size_t dwLng) {
    for (; dwLng != 0; dwLng--) {
        dwCRC = ComputeCRC32_Byte(dwCRC, *pbBuf);
        pbBuf++;
    }
    return (dwCRC);
}
