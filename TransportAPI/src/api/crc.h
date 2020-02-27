#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t ComputeCRC32(uint32_t dwCRC, const uint8_t *pbBuf, size_t dwLng);

#ifdef __cplusplus
}
#endif
