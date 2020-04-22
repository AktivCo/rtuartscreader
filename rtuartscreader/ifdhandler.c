// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <PCSC/ifdhandler.h>
#include <PCSC/reader.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "reader.h"
#include "reader_list.h"

RESPONSECODE IFDHCreateChannelByName(DWORD Lun, LPSTR DeviceName) {
    init_log();

    LOG_INFO("Lun: %lu, DeviceName: %s", Lun, DeviceName);

    Reader* reader = NULL;

    reader = reader_list_get_reader(Lun);
    if (reader) {
        LOG_CRITICAL_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Lun is already in use");
    }

    reader = reader_list_alloc_reader(Lun);
    if (!reader) {
        LOG_CRITICAL_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Failed to alloc reader");
    }

    reader_status_t r = reader_open(reader, DeviceName);
    if (r != reader_status_ok) {
        reader_list_free_reader(Lun);

        if (r == reader_status_reader_not_found) {
            LOG_CRITICAL_RETURN_IFD(IFD_NO_SUCH_DEVICE, "reader_open failed: %d", r);
        } else {
            LOG_CRITICAL_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_open failed: %d", r);
        }
    }

    LOG_INFO_RETURN_IFD(IFD_SUCCESS);
}

RESPONSECODE IFDHControl(DWORD Lun, DWORD dwControlCode, PUCHAR TxBuffer, DWORD TxLength, PUCHAR RxBuffer,
                         DWORD RxLength, LPDWORD pdwBytesReturned) {
    LOG_INFO("Lun: %lu, dwControlCode: %lu", Lun, dwControlCode);
    LOG_INFO_RETURN_IFD(IFD_NOT_SUPPORTED);
}

RESPONSECODE IFDHCreateChannel(DWORD Lun, DWORD Channel) {
    LOG_INFO("Lun: %lu, Channel: %lu", Lun, Channel);

    char deviceName[256] = { 0 };

    if (snprintf(deviceName, sizeof(deviceName), "/dev/pcsc/%lu", Channel) < 0) {
        LOG_CRITICAL_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Failed to snprintf");
    }

    return IFDHCreateChannelByName(Lun, deviceName);
}

RESPONSECODE IFDHCloseChannel(DWORD Lun) {
    LOG_INFO("Lun: %lu", Lun);

    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid Lun");
    }

    reader_status_t r = reader_power_off(reader);
    if (r != reader_status_ok) {
        LOG_ERROR("reader_power_off failed: %d", r);
    }

    r = reader_close(reader);
    if (r != reader_status_ok) {
        LOG_ERROR("reader_close failed: %d", r);
    }

    reader_list_free_reader(Lun);

    LOG_INFO_RETURN_IFD(IFD_SUCCESS);
}

RESPONSECODE GetCapability(DWORD Length, const UCHAR* value, PDWORD targetLength, PUCHAR targetValue) {
    if (!targetLength) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Value buffer ptr is NULL");
    }

    if (*targetLength < Length) {
        *targetLength = Length;
        LOG_INFO_RETURN_IFD(IFD_ERROR_INSUFFICIENT_BUFFER);
    }

    *targetLength = Length;

    if (!targetValue) {
        LOG_INFO_RETURN_IFD(IFD_SUCCESS);
    }

    memcpy(targetValue, value, Length);

    LOG_INFO_RETURN_IFD(IFD_SUCCESS);
}

RESPONSECODE IFDHGetCapabilities(DWORD Lun, DWORD Tag, PDWORD Length, PUCHAR Value) {
    LOG_INFO("Lun: %lu, Tag: 0x%lx", Lun, Tag);

    switch (Tag) {
    case TAG_IFD_ATR:
    case SCARD_ATTR_ATR_STRING: {
        Reader* reader = reader_list_get_reader(Lun);
        if (!reader) {
            LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid Lun");
        }

        const UCHAR* atr;
        DWORD atrLen;
        reader_status_t r = reader_get_atr(reader, &atr, &atrLen);
        if (r != reader_status_ok) {
            LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_get_atr failed %d", r);
        }

        return GetCapability(atrLen, atr, Length, Value);
    }
    case TAG_IFD_SIMULTANEOUS_ACCESS: {
        UCHAR result = gReaderListSize;
        return GetCapability(1, &result, Length, Value);
    }
    case TAG_IFD_THREAD_SAFE: {
        UCHAR result = 0; // No thread-safety
        return GetCapability(1, &result, Length, Value);
    }
    case TAG_IFD_SLOTS_NUMBER: {
        UCHAR result = 1; // Single slot
        return GetCapability(1, &result, Length, Value);
    }
    case TAG_IFD_SLOT_THREAD_SAFE: {
        UCHAR result = 0; // No thread-safety
        return GetCapability(1, &result, Length, Value);
    }
    case TAG_IFD_POLLING_THREAD:
    case TAG_IFD_POLLING_THREAD_WITH_TIMEOUT:
    case TAG_IFD_POLLING_THREAD_KILLABLE:
    case TAG_IFD_STOP_POLLING_THREAD:
        LOG_INFO_RETURN_IFD(IFD_NOT_SUPPORTED);
    default:
        LOG_INFO_RETURN_IFD(IFD_ERROR_TAG);
    }
}

RESPONSECODE IFDHSetCapabilities(DWORD Lun, DWORD Tag, DWORD Length, PUCHAR Value) {
    LOG_INFO("Lun: %lu, Tag: %lu", Lun, Tag);
    LOG_INFO_RETURN_IFD(IFD_NOT_SUPPORTED);
}

RESPONSECODE IFDHSetProtocolParameters(DWORD Lun, DWORD Protocol, UCHAR Flags, UCHAR PTS1, UCHAR PTS2, UCHAR PTS3) {
    LOG_INFO("Lun: %lu, Protocol: %lu", Lun, Protocol);
    LOG_INFO_RETURN_IFD(IFD_NOT_SUPPORTED);
}

RESPONSECODE IFDHPowerICC(DWORD Lun, DWORD Action, PUCHAR Atr, PDWORD AtrLength) {
    LOG_INFO("Lun: %lu, Action: 0x%lx", Lun, Action);

    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid Lun");
    }

    reader_status_t r;

    const UCHAR* atr;
    DWORD atrLen;

    switch (Action) {
    case IFD_POWER_UP:
        r = reader_power_on(reader, &atr, &atrLen);
        if (r != reader_status_ok) {
            LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_power_on failed: %d", r);
        }

        return GetCapability(atrLen, atr, AtrLength, Atr);
    case IFD_RESET:
        r = reader_reset(reader, &atr, &atrLen);
        if (r != reader_status_ok) {
            LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_get_atr failed: %d", r);
        }

        return GetCapability(atrLen, atr, AtrLength, Atr);
    case IFD_POWER_DOWN:
        r = reader_power_off(reader);
        if (r != reader_status_ok) {
            LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_power_off failed: %d", r);
        }
        LOG_INFO_RETURN_IFD(IFD_SUCCESS);
    default:
        LOG_ERROR_RETURN_IFD(IFD_ERROR_NOT_SUPPORTED, "Action: %lx", Action);
    }
}

RESPONSECODE IFDHTransmitToICC(DWORD Lun, SCARD_IO_HEADER SendPci, PUCHAR TxBuffer, DWORD TxLength, PUCHAR RxBuffer,
                               PDWORD RxLength, PSCARD_IO_HEADER RecvPci) {
    LOG_INFO("Lun: %lu", Lun);

    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid Lun");
    }

    reader_status_t r = reader_transmit(reader, TxBuffer, TxLength, RxBuffer, RxLength);
    if (r != reader_status_ok) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_transmit failed: %d", r);
    }

    LOG_INFO_RETURN_IFD(IFD_SUCCESS);
}

RESPONSECODE IFDHICCPresence(DWORD Lun) {
    LOG_DEBUG("Lun: %lu", Lun);

    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid Lun");
    }

    reader_status_t r = reader_is_present(reader);
    if (r == reader_status_reader_not_found) {
        LOG_INFO_RETURN_IFD(IFD_ICC_NOT_PRESENT);
    } else if (r != reader_status_ok) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "reader_is_present failed: %d", r);
    }

    LOG_DEBUG_RETURN_IFD(IFD_SUCCESS);
}
