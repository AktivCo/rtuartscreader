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
