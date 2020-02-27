#include <TransportAPI/TransportAPI.h>

#include <stdio.h>
#include <string.h>

#include "com.h"
#include "transport.h"
#include "utils.h"

/*
 * Initialize COM device
 */
io_status_t
TrAPI_OpenDevice(
    PDEVICE_HANDLE phDev,
    const char *acComPortName,
    uint32_t BaudRate) {
    DEVICE_HANDLE h = { 0 };

    TrAPI_SetPacketNum(&h, 1);

    io_status_t ret;

    ret = com_init(&h.hPort, acComPortName, BaudRate);

    if (ret != IO_ERROR__OK) {
        perror("COM port init failed: ");

        return ret;
    }

    ret = TrAPI_Synchronise(&h);

    if (ret != IO_ERROR__OK) {
        perror("TrAPI_Synchronise failed: ");

        return ret;
    }

    *phDev = h;

    return ret;
}

io_status_t TrAPI_CloseDevice(
    PDEVICE_HANDLE phDev) {
    int ret;

    ret = com_deinit(phDev->hPort);

    if (ret) {
        perror("COM port deinit failed: ");
    }

    return ret;
}

/*
 * In\Out
 */
static io_status_t
TrAPI_TransmitEx(
    PDEVICE_HANDLE phDev,
    uint32_t dwCmdType,
    const uint8_t *pBufSend,
    uint16_t dwLngBufSend,
    uint8_t *pBufRecv,
    uint16_t *pdwLngBufRecv) {
    // Sending/recieving data:
    io_status_t dwRet;
    TRANSPORT_PACKET_HEADER hdr;
    TRANSPORT_PACKET_HEADER hdr_rcv;
    uint8_t hdrData[sizeof(TRANSPORT_PACKET_HEADER)];
    uint32_t dwTryCount;
    uint16_t dwLngBufRecv = 0;

    if ((pBufSend == NULL && dwLngBufSend != 0) || pdwLngBufRecv == NULL) {
        perror("TrAPI_TransmitEx failed: wrong args");

        return (IO_ERROR__INVALID_ARG);
    }

    memset(&hdr, 0, sizeof(hdr));

    hdr.bHdrSize = sizeof(hdr);
    hdr.bHdrVers = TRASPORT_HEADER_VERS;
    hdr.bProtocolVers = TRASPORT_PROTOCOL_VERS;
    hdr.bPID = dwCmdType;
    hdr.wPacketNum = TrAPI_GetPacketNum(phDev);
    hdr.wSize = dwLngBufSend;
    hdr.dwDataCrc32 = TrAPI_ComputeCRC(&hdr, pBufSend, dwLngBufSend);

    pack_header(&hdr, hdrData);

    for (dwTryCount = 0;; dwTryCount++) {
        if (dwTryCount >= IO_TRY_NUM)
            break;
        dwRet = TrAPI_SendFrame(phDev, hdrData, sizeof(TRANSPORT_PACKET_HEADER), pBufSend, dwLngBufSend);
        if (dwRet == IO_ERROR__INVALID_ARG || dwRet == IO_ERROR__NO_MEMORY) {
            perror("TrAPI_SendFrame failed: ");

            break;
        }
        if (dwRet != IO_ERROR__OK)
            continue;

        for (;;) {
            dwLngBufRecv = *pdwLngBufRecv;
            memset(&hdr_rcv, 0, sizeof(hdr_rcv));
            dwRet = TrAPI_RecvFrame(phDev, &hdr_rcv, pBufRecv, &dwLngBufRecv);
            if (dwRet == IO_ERROR__OK) {
                // Packet number increasing
                if (hdr_rcv.bRet != 0)
                    dwRet = hdr_rcv.bRet;
                if (dwRet != IO_ERROR__WAIT) {
                    TrAPI_PacketNumInc(phDev);
                }
            }
            if (dwRet == IO_ERROR__WAIT)
                continue;
            break;
        }
        if (dwRet == IO_ERROR__INVALID_ARG || dwRet == IO_ERROR__NO_MEMORY) {
            perror("TrAPI_RecvFrame failed: ");

            break;
        }
        if (dwRet == IO_ERROR__OK)
            break;
    }
    *pdwLngBufRecv = dwLngBufRecv;
    return (dwRet);
}

/*
 * In\Out
 */
io_status_t
TrAPI_Transmit(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufSend,
    uint16_t dwLngBufSend,
    uint8_t *pBufRecv,
    uint16_t *pdwLngBufRecv) {
    return (TrAPI_TransmitEx(phDev, TRANSPORT_CMD__TRANSMIT, pBufSend, dwLngBufSend, pBufRecv, pdwLngBufRecv));
}

/*
 * Synchronize Packet
 */
io_status_t
TrAPI_Synchronise(
    PDEVICE_HANDLE phDev) {
    uint16_t dwLngBufRecv = 0;

    return (TrAPI_TransmitEx(phDev, TRANSPORT_CMD__SYNCHRONISE, NULL, 0, NULL, &dwLngBufRecv));
}