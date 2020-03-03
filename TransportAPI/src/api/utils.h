// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include "transport.h"

#ifdef __cplusplus
extern "C" {
#endif

//Packet header serialization
void pack_header(PCTRANSPORT_PACKET_HEADER pHdr, uint8_t *hdrData);
void unpack_header(PTRANSPORT_PACKET_HEADER pHdr, const uint8_t *hdrData);
void pack_header_null_crc(PCTRANSPORT_PACKET_HEADER pHdr, uint8_t *hdrData);

#ifdef __cplusplus
}
#endif
