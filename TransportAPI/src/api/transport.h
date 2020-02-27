#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <TransportAPI/types.h>

#define FRAME_LNG_SYMBOL_START 1
#define FRAME_LNG_SYMBOL_END 1

#define IO_TRY_NUM 3
#define IO_WAIT_TIME_PACKETS 1000

// Exchange protocol version
#define TRASPORT_PROTOCOL_VERS 1
#define TRASPORT_HEADER_VERS 1

#define TRANSPORT_CMD__SYNCHRONISE 1
#define TRANSPORT_CMD__TRANSMIT 2
#define TRANSPORT_CMD__CLOSE 3
#define TRANSPORT_CMD__RESET 4

// Transport protocol v.1
typedef struct {
    uint8_t bHdrSize;
    uint8_t bHdrVers;
    uint8_t bProtocolVers;
    uint8_t bPID;
    uint32_t dwDataCrc32;
    uint16_t wPacketNum;
    uint16_t wSize;
    uint8_t bRet;
    uint8_t abRes[3];
} TRANSPORT_PACKET_HEADER, *PTRANSPORT_PACKET_HEADER;

#define TRANSPORT_PACKET_HEADER_VER1 TRANSPORT_PACKET_HEADER

// Common functions
void TrAPI_SetPacketNum(
    PDEVICE_HANDLE phDev,
    uint16_t wPacketNum);

uint16_t
TrAPI_GetPacketNum(
    PDEVICE_HANDLE phDev);

void TrAPI_PacketNumInc(
    PDEVICE_HANDLE hDev);

uint32_t
TrAPI_ComputeCRC(
    const PTRANSPORT_PACKET_HEADER aHdr,
    const uint8_t *aBuf,
    uint32_t dwLng);

io_status_t
TrAPI_SendFrame(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufHdr,
    uint16_t dwLngHdr,
    const uint8_t *pBufData,
    uint16_t dwLngData);

io_status_t
TrAPI_RecvFrame(
    PDEVICE_HANDLE phDev,
    PTRANSPORT_PACKET_HEADER pHdr,
    uint8_t *pBuf,
    uint16_t *realsize);

io_status_t TrAPI_SendByte(PDEVICE_HANDLE phDev, uint8_t bCh);
io_status_t TrAPI_SendByteMasked(PDEVICE_HANDLE phDev, uint8_t bCh);

#ifdef __cplusplus
}
#endif
