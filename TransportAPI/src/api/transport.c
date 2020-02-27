#include "transport.h"

#include <string.h>
#include <unistd.h>

#include "crc.h"
#include "utils.h"

uint32_t
TrAPI_ComputeCRC(
    const PTRANSPORT_PACKET_HEADER pHdr,
    const uint8_t *aBuf,
    uint32_t dwLng) {
    uint32_t dwCRC = 0;
    uint8_t hdrData[sizeof(TRANSPORT_PACKET_HEADER)];
    uint32_t dwCRC_backup;

    dwCRC_backup = pHdr->dwDataCrc32;

    pHdr->dwDataCrc32 = 0;
    pack_header(pHdr, hdrData);
    dwCRC = ComputeCRC32(0, hdrData, pHdr->bHdrSize);
    pHdr->dwDataCrc32 = dwCRC_backup;

    if (aBuf != NULL && dwLng != 0)
        dwCRC = ComputeCRC32(dwCRC, aBuf, dwLng);
    return dwCRC;
}

io_status_t TrAPI_RecvByte(PDEVICE_HANDLE phDev, uint8_t *pbCh) {
    uint32_t dwTimeout = 0;

    while (1) {
        ssize_t dwRealSize;

        if ((dwRealSize = read(phDev->hPort, pbCh, 1)) == -1)
            return IO_ERROR__PROTOCOL;

        if (!dwRealSize) {
            dwTimeout++;
            if ((dwTimeout * 50) > IO_WAIT_TIME_PACKETS)
                return IO_ERROR__TIMEOUT;
        } else
            break;
    }
    return IO_ERROR__OK;
}

io_status_t TrAPI_SendByte(PDEVICE_HANDLE phDev, uint8_t bCh) {
    ssize_t dwRealSize;
    if ((dwRealSize = write(phDev->hPort, &bCh, 1)) == -1) {
        return IO_ERROR__PROTOCOL;
    }
    if (!dwRealSize) {
        return IO_ERROR__PROTOCOL;
    }
    return IO_ERROR__OK;
}

io_status_t TrAPI_SendByteMasked(PDEVICE_HANDLE phDev, uint8_t bCh) {
    io_status_t dwRet = 0;
    switch (bCh) {
    case '\\':
    case '$':
        dwRet = TrAPI_SendByte(phDev, '\\');
        break;
    case ':':
        dwRet = TrAPI_SendByte(phDev, '\\');
        bCh = 'c';
        break;
    case '\n':
        dwRet = TrAPI_SendByte(phDev, '\\');
        bCh = 'n';
        break;
    default:
        break;
    }
    if (dwRet) return dwRet;
    dwRet = TrAPI_SendByte(phDev, bCh);
    return dwRet;
}

/*
 * Send packet
 */
io_status_t
TrAPI_SendFrame(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufHdr,
    uint16_t dwLngHdr,
    const uint8_t *pBufData,
    uint16_t dwLngData) {
    io_status_t dwRet = 0;
    int i;

    // Send beginning marker:
    dwRet = TrAPI_SendByte(phDev, ':');
    if (dwRet) return dwRet;

    for (i = 0; i < dwLngHdr; i++) {
        dwRet = TrAPI_SendByteMasked(phDev, (pBufHdr)[i]);
        if (dwRet) return dwRet;
    }
    for (i = 0; i < dwLngData; i++) {
        dwRet = TrAPI_SendByteMasked(phDev, (pBufData)[i]);
        if (dwRet) return dwRet;
    }

    // Send ending marker:
    dwRet = TrAPI_SendByte(phDev, '\n');

    return dwRet;
}

void TrAPI_SetPacketNum(
    PDEVICE_HANDLE phDev,
    uint16_t wPacketNum) {
    phDev->wPacketNum = wPacketNum;
}

uint16_t
TrAPI_GetPacketNum(
    PDEVICE_HANDLE phDev) {
    return phDev->wPacketNum;
}

void TrAPI_PacketNumInc(
    PDEVICE_HANDLE hDev) {
    PDEVICE_HANDLE hDevTbl = hDev;

    hDevTbl->wPacketNum++;
}

#define PACKET_MAX_BODY_SIZE (5 + 255 + 1)
#define PACKET_MAX_SIZE (sizeof(TRANSPORT_PACKET_HEADER) + FRAME_LNG_SYMBOL_START + FRAME_LNG_SYMBOL_END + PACKET_MAX_BODY_SIZE)

/*
 * Receive an answer
 */
io_status_t
TrAPI_RecvFrame(
    PDEVICE_HANDLE phDev,
    PTRANSPORT_PACKET_HEADER pHdr,
    uint8_t *pBuf,
    uint16_t *realsize) {
    io_status_t dwRet;
    uint32_t offset = 0;
    uint32_t dwLng = 0;
    uint32_t dwSOF_Ok = 0;
    uint32_t dwEOF_Ok = 0;
    uint32_t dwHdr_Ok = 0;

    uint8_t buff_recv[PACKET_MAX_SIZE];

    while (1) {
        dwRet = TrAPI_RecvByte(phDev, &buff_recv[offset]);
        if (dwRet != IO_ERROR__OK) {
            goto exit;
        }

        if (buff_recv[offset] == ':') {
            dwSOF_Ok = 1;
            dwHdr_Ok = 0;
            offset = 1;
            continue;
        } else if (buff_recv[offset] == '\n') {
            dwEOF_Ok = 1;
            offset++;
        } else {
            if (buff_recv[offset] == '\\') {
                dwRet = TrAPI_RecvByte(phDev, &buff_recv[offset]);
                if (dwRet != IO_ERROR__OK) {
                    goto exit;
                }

                switch (buff_recv[offset]) {
                case 'c':
                    buff_recv[offset] = ':';
                    break;
                case 'n':
                    buff_recv[offset] = '\n';
                    break;
                }
            }
            if (dwSOF_Ok) {
                // only if SOF-byte has received
                offset++;

                if (offset >= PACKET_MAX_SIZE) {
                    dwRet = IO_ERROR__DATA_LEN;
                    goto exit;
                }
            }
        }

        if (dwHdr_Ok == 0 && offset >= (FRAME_LNG_SYMBOL_START + sizeof(TRANSPORT_PACKET_HEADER) + FRAME_LNG_SYMBOL_END)) {
            // Header has received
            dwHdr_Ok = 1;

            unpack_header(pHdr, &buff_recv[FRAME_LNG_SYMBOL_START]);
            if (pHdr->wSize > *realsize) {
                dwRet = IO_ERROR__PACKET_LEN;
                goto exit;
            }
            if (pHdr->bHdrSize < sizeof(TRANSPORT_PACKET_HEADER_VER1)) {
                // header size can't be lower than size of v.1 header
                dwRet = IO_ERROR__PACKET_LEN;
                goto exit;
            }
            dwLng = pHdr->bHdrSize + pHdr->wSize + FRAME_LNG_SYMBOL_START + FRAME_LNG_SYMBOL_END;
            if (dwLng > PACKET_MAX_SIZE) {
                dwRet = IO_ERROR__DATA_LEN;
                goto exit;
            }
        }

        if (dwEOF_Ok) {
            dwLng = offset - FRAME_LNG_SYMBOL_START - FRAME_LNG_SYMBOL_END;
            break;
        }
    }

    // Received data checks
    // + Size checking
    // + CRC checking
    // + Header data checking:
    //   + Header version checking
    //   + Protocol version checking
    //   + Packet number checking
    //   + Packet type checking
    //   + Packet number increasing

    // Header size checking
    *realsize = 0;
    if (dwLng < sizeof(TRANSPORT_PACKET_HEADER))
        dwRet = IO_ERROR__PACKET_LEN;
    else if (dwLng != (pHdr->bHdrSize + pHdr->wSize))
        dwRet = IO_ERROR__DATA_LEN;

    // CRC checking
    if (dwRet == IO_ERROR__OK) {
        if (pHdr->dwDataCrc32 != TrAPI_ComputeCRC(pHdr, &buff_recv[FRAME_LNG_SYMBOL_START + pHdr->bHdrSize], pHdr->wSize))
            dwRet = IO_ERROR__CRC;
    }

    // Header version checking
    if (dwRet == IO_ERROR__OK) {
        if (pHdr->bHdrVers == 0)
            dwRet = IO_ERROR__INVALID_HDR_INFO;
    }

    // Protocol version checking
    if (dwRet == IO_ERROR__OK) {
        if (pHdr->bProtocolVers > TRASPORT_PROTOCOL_VERS)
            dwRet = IO_ERROR__INVALID_PROTOCOL_NUM;
    }

    // Packet number checking
    if (dwRet == IO_ERROR__OK) {
        if (TrAPI_GetPacketNum(phDev) != pHdr->wPacketNum)
            dwRet = IO_ERROR__PACKET_NUM;
    }

    // Packet type checking
    if (dwRet == IO_ERROR__OK) {
        if (pHdr->bPID != TRANSPORT_CMD__SYNCHRONISE && //
            pHdr->bPID != TRANSPORT_CMD__TRANSMIT &&    //
            pHdr->bPID != TRANSPORT_CMD__CLOSE &&       //
            pHdr->bPID != TRANSPORT_CMD__RESET)
            dwRet = IO_ERROR__INVALID_HDR_INFO;
    }

    if (dwRet == IO_ERROR__OK) {
        *realsize = pHdr->wSize;
        if (pBuf != NULL) {
            memcpy((void *)pBuf, &buff_recv[FRAME_LNG_SYMBOL_START + sizeof(TRANSPORT_PACKET_HEADER)], pHdr->wSize);
        }
    }
exit:

    return (dwRet);
}
