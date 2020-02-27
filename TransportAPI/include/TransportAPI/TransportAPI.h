#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "types.h"

/*
 * TrAPI_Synchronise()
 * Send syncronization packet to device
 */
io_status_t
TrAPI_Synchronise(
    PDEVICE_HANDLE phDev);

/*
 * TrAPI_OpenDevice()
 * Initialize COM device
 */
io_status_t
TrAPI_OpenDevice(
    PDEVICE_HANDLE phDev,
    const char *acComPortName,
    uint32_t BaudRate);

/*
 * TrAPI_CloseDevice()
 * Deinitialize COM device
 */
io_status_t TrAPI_CloseDevice(
    PDEVICE_HANDLE phDev);

/*
 * TrAPI_Transmit()
 * Send an APDU command for device and receive answer. In\Out
 */
io_status_t
TrAPI_Transmit(
    PDEVICE_HANDLE phDev,
    const uint8_t *pBufSend,
    uint16_t dwLngBufSend,
    uint8_t *pBufRecv,
    uint16_t *pdwLngBufRecv);

io_status_t
TrAPI_Reset(
    PDEVICE_HANDLE phDev);

#ifdef __cplusplus
}
#endif
