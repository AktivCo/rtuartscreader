// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <PCSC/ifdhandler.h>
#include <PCSC/reader.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <rtuartscreader/ifdhandler_log.h>
#include <rtuartscreader/log/init.h>
#include <rtuartscreader/log/log.h>
#include <rtuartscreader/reader.h>
#include <rtuartscreader/reader_list.h>

static const char* ifd_error_to_string(int error) {
    switch (error) {
    case IFD_SUCCESS: return "IFD_SUCCESS";
    case IFD_ERROR_TAG: return "IFD_ERROR_TAG";
    case IFD_ERROR_SET_FAILURE: return "IFD_ERROR_SET_FAILURE";
    case IFD_ERROR_VALUE_READ_ONLY: return "IFD_ERROR_VALUE_READ_ONLY";
    case IFD_ERROR_PTS_FAILURE: return "IFD_ERROR_PTS_FAILURE";
    case IFD_ERROR_NOT_SUPPORTED: return "IFD_ERROR_NOT_SUPPORTED";
    case IFD_PROTOCOL_NOT_SUPPORTED: return "IFD_PROTOCOL_NOT_SUPPORTED";
    case IFD_ERROR_POWER_ACTION: return "IFD_ERROR_POWER_ACTION";
    case IFD_ERROR_SWALLOW: return "IFD_ERROR_SWALLOW";
    case IFD_ERROR_EJECT: return "IFD_ERROR_EJECT";
    case IFD_ERROR_CONFISCATE: return "IFD_ERROR_CONFISCATE";
    case IFD_COMMUNICATION_ERROR: return "IFD_COMMUNICATION_ERROR";
    case IFD_RESPONSE_TIMEOUT: return "IFD_RESPONSE_TIMEOUT";
    case IFD_NOT_SUPPORTED: return "IFD_NOT_SUPPORTED";
    case IFD_ICC_PRESENT: return "IFD_ICC_PRESENT";
    case IFD_ICC_NOT_PRESENT: return "IFD_ICC_NOT_PRESENT";
    case IFD_NO_SUCH_DEVICE: return "IFD_NO_SUCH_DEVICE";
    case IFD_ERROR_INSUFFICIENT_BUFFER: return "IFD_ERROR_INSUFFICIENT_BUFFER";
    default: return "unknown";
    }
}

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

    if (snprintf(deviceName, sizeof(deviceName), "/dev/pcsc/%lu", (unsigned long int)Channel) < 0) {
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

static RESPONSECODE ZeroAtr(PUCHAR Atr, PDWORD AtrLength) {
    if (!AtrLength) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid AtrLength ptr");
    }
    if (!Atr) {
        LOG_ERROR_RETURN_IFD(IFD_COMMUNICATION_ERROR, "Invalid Atr ptr");
    }

    memset(Atr, 0, *AtrLength * sizeof(Atr[0]));
    *AtrLength = 0;

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

        reader_status_t r = reader_is_powered(reader);
        if (r == reader_status_reader_unpowered) {
            return ZeroAtr(Value, Length);
        }

        const UCHAR* atr;
        DWORD atrLen;
        r = reader_get_atr(reader, &atr, &atrLen);
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

        // pcsclite reference: IFD_POWER_DOWN
        // Power down the card (Atr and AtrLength should be zeroed)
        return ZeroAtr(Atr, AtrLength);
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

static RESPONSECODE doIFDHICCPresence(DWORD Lun) {
    LOG_INFO("Lun: %lu", Lun);

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

    LOG_INFO_RETURN_IFD(IFD_SUCCESS);
}

RESPONSECODE IFDHICCPresence(DWORD Lun) {
    log_level_t currentLogLevel = log_get_log_level();
    if ((currentLogLevel & LOG_LEVEL_PERIODIC) != LOG_LEVEL_PERIODIC) {
        log_set_log_level(LOG_LEVEL_CRITICAL);
    }

    RESPONSECODE r = doIFDHICCPresence(Lun);

    log_set_log_level(currentLogLevel);

    return r;
}
