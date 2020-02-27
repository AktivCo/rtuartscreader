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
