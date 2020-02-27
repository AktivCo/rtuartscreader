#include "TransportApiTest.h"

#include <string.h>
#include <algorithm>
#include <random>
#include <type_traits>
#include <vector>

#include "../api/utils.h"
#include "../api/transport.h"

#include "ConsoleColor.h"

static uint32_t TrAPI_Test_SendFrame(PDEVICE_HANDLE phDev, const uint8_t *pBufHdr, uint32_t dwLngHdr, const uint8_t *pBufData, uint32_t dwLngData, SetTestError_t Error);
static uint32_t TrAPI_Test_SendByteMasked(PDEVICE_HANDLE phDev, uint8_t bCh, SetTestError_t Error);
static uint32_t TrAPI_Test_GetRandom(uint32_t Exclude, uint32_t SizeType);
/*!************************************************************************
* @brief Start of refactoring
* @param
* @return
*************************************************************************/
template<bool U>
using bool_constant = std::integral_constant<bool, U>;

template<typename T>
static T TrAPI_Test_GetRandom(std::vector<T> &Exclude, bool_constant<false>);

template<typename T>
static T TrAPI_Test_GetRandom(std::vector<T> &Exclude, bool_constant<true>) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    T Gen;
    std::uniform_int_distribution<T> dis(0);
    do {
        Gen = dis(gen);
    } while (std::find(Exclude.begin(), Exclude.end(), Gen) != Exclude.end());
    return Gen;
}

template<typename T>
static T TrAPI_Test_GetRandom(std::vector<T> &Exclude) {
    return TrAPI_Test_GetRandom(Exclude, bool_constant<std::is_integral<T>::value>());
}

/*!************************************************************************
* @brief
* @param
* @return
*************************************************************************/
uint32_t
TrAPI_Test_TransmitEx(
    PDEVICE_HANDLE phDev,
    uint32_t dwCmdType,
    const uint8_t *pBufSend,
    uint16_t dwLngBufSend,
    uint8_t *pBufRecv,
    uint16_t *pdwLngBufRecv,
    SetTestError_t Error) {
    uint32_t dwRet;
    TRANSPORT_PACKET_HEADER hdr;
    TRANSPORT_PACKET_HEADER hdr_rcv;
    uint8_t buff[sizeof(TRANSPORT_PACKET_HEADER)];

    uint32_t dwTryCount;
    uint16_t dwLngBufRecv = 0;
    uint32_t timeout = 0;

    if ((pBufSend == NULL && dwLngBufSend != 0) || pdwLngBufRecv == NULL)
        return (IO_ERROR__INVALID_ARG);

    memset(&hdr, 0, sizeof(hdr));

    std::vector<uint8_t> exclude_hdr_size = { sizeof(hdr) };
    hdr.bHdrSize = (Error & ST_HdrSize) ? (uint8_t)TrAPI_Test_GetRandom(exclude_hdr_size) : sizeof(hdr);

    std::vector<uint8_t> exclude_hdr_version = { TRASPORT_HEADER_VERS };
    hdr.bHdrVers = (Error & ST_HdrVers) ? (uint8_t)TrAPI_Test_GetRandom(exclude_hdr_version) : TRASPORT_HEADER_VERS;

    std::vector<uint8_t> exclude_prot_versions = { 0, 1 };
    hdr.bProtocolVers = (Error & ST_ProtocolVers) ? (uint8_t)TrAPI_Test_GetRandom(exclude_prot_versions) : TRASPORT_PROTOCOL_VERS;

    std::vector<uint8_t> exclude_cmd_type = { (uint8_t)dwCmdType };
    hdr.bPID = (Error & ST_PID) ? (uint8_t)TrAPI_Test_GetRandom(exclude_cmd_type) : (uint8_t)dwCmdType;

    std::vector<uint16_t> exclude_packet_num = { TrAPI_GetPacketNum(phDev) };
    hdr.wPacketNum = (Error & ST_PacketNum) ? (uint8_t)TrAPI_Test_GetRandom(exclude_packet_num) : TrAPI_GetPacketNum(phDev);

    std::vector<uint16_t> exclude_data_size = { dwLngBufSend };
    hdr.wSize = (Error & ST_Size) ? (uint16_t)TrAPI_Test_GetRandom(exclude_data_size) : dwLngBufSend;

    std::vector<uint32_t> exclude_crc = { TrAPI_ComputeCRC(&hdr, pBufSend, dwLngBufSend) };
    hdr.dwDataCrc32 = (Error & ST_DataCrc) ? (uint32_t)TrAPI_Test_GetRandom(exclude_crc) : TrAPI_ComputeCRC(&hdr, pBufSend, dwLngBufSend);

    pack_header(&hdr, buff);

    for (dwTryCount = 0;; dwTryCount++) {
        if (dwTryCount >= IO_TRY_NUM)
            break;

        log_info("Sending data (attempt %d): ", dwTryCount);

        dwRet = TrAPI_Test_SendFrame(phDev, buff, sizeof(TRANSPORT_PACKET_HEADER), pBufSend, dwLngBufSend, Error);

        log_info("\r\n");

        if (dwRet == IO_ERROR__INVALID_ARG || dwRet == IO_ERROR__NO_MEMORY)
            break;
        if (dwRet != 0)
            continue;

        timeout = 0;

        for (;;) {
            dwLngBufRecv = *pdwLngBufRecv;
            memset(&hdr_rcv, 0, sizeof(hdr_rcv));

            dwRet = TrAPI_RecvFrame(phDev, &hdr_rcv, pBufRecv, &dwLngBufRecv);

            if (dwRet == 0) {
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
        if (dwRet == IO_ERROR__INVALID_ARG || dwRet == IO_ERROR__NO_MEMORY)
            break;
        if (dwRet == 0) {
            if (dwLngBufRecv)
                log_buf("Recieved data: ", pBufRecv, dwLngBufRecv);

            break;
        }
    }
    *pdwLngBufRecv = dwLngBufRecv;
    return (dwRet);
}
/*!************************************************************************
* @brief
* @param
* @return
*************************************************************************/
static uint32_t TrAPI_Test_SendFrame(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufHdr,
    uint32_t dwLngHdr,
    const uint8_t *pBufData,
    uint32_t dwLngData,
    SetTestError_t Error) {
    uint32_t dwRet = 0;
    int i;
    int ret;

    // Sending initial control character
    if (!(Error & ST_NoStart)) {
        log_info("%02X ", ':');
        ret = TrAPI_SendByte(phDev, ':');
    } else {
        ret = 0;
    }
    if (ret) return ret;

    for (i = 0; i < dwLngHdr; i++) {
        ret = TrAPI_Test_SendByteMasked(phDev, (pBufHdr)[i], Error);
        if (ret) return ret;
    }
    for (i = 0; i < dwLngData; i++) {
        ret = TrAPI_Test_SendByteMasked(phDev, (pBufData)[i], Error);
        if (ret) return ret;
    }

    // Sending last control character
    if (!(Error & ST_NoEnd)) {
        log_info("%02X ", '\n');
        ret = TrAPI_SendByte(phDev, '\n');
    }
    if (ret) return ret;

    return (dwRet);
}
/*!************************************************************************
* @brief
* @param
* @return
*************************************************************************/
static uint32_t TrAPI_Test_SendByteMasked(PDEVICE_HANDLE phDev, uint8_t bCh, SetTestError_t Error) {
    uint32_t dwRet = 0;

    if (!(Error & ST_NoEscape)) {
        switch (bCh) {
        case '\\':
        case '$':
            log_info("%02X ", '\\');
            dwRet = TrAPI_SendByte(phDev, '\\');
            break;
        case ':':
            log_info("%02X ", '\\');
            dwRet = TrAPI_SendByte(phDev, '\\');
            bCh = 'c';
            break;
        case '\n':
            log_info("%02X ", '\\');
            dwRet = TrAPI_SendByte(phDev, '\\');
            bCh = 'n';
            break;
        default:
            break;
        }
    }
    if (dwRet) return dwRet;
    log_info("%02X ", bCh);
    dwRet = TrAPI_SendByte(phDev, bCh);
    return dwRet;
}
/*!************************************************************************
* @brief
* @param
* @return
*************************************************************************/
uint32_t
TrAPI_Test_Transmit(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufSend,
    uint16_t dwLngBufSend,
    uint8_t *pBufRecv,
    uint16_t *pdwLngBufRecv,
    SetTestError_t Error) {
    return (TrAPI_Test_TransmitEx(phDev, TRANSPORT_CMD__TRANSMIT, pBufSend, dwLngBufSend, pBufRecv, pdwLngBufRecv, Error));
}
/*!************************************************************************
* @brief
* @param
* @return
*************************************************************************/
