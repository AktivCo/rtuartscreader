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
        return IFD_COMMUNICATION_ERROR;
    }

    reader = reader_list_alloc_reader(Lun);
    if (!reader) {
        return IFD_COMMUNICATION_ERROR;
    }

    reader_status_t r = reader_open(reader, DeviceName);
    if (r != reader_status_ok) {
        reader_list_free_reader(Lun);

        if (r == reader_status_reader_not_found)
            return IFD_NO_SUCH_DEVICE;
        else
            return IFD_COMMUNICATION_ERROR;
    }

    return IFD_SUCCESS;
}

RESPONSECODE IFDHControl(DWORD Lun, DWORD dwControlCode, PUCHAR TxBuffer, DWORD TxLength, PUCHAR RxBuffer,
                         DWORD RxLength, LPDWORD pdwBytesReturned) {
    return IFD_NOT_SUPPORTED;
}

RESPONSECODE IFDHCreateChannel(DWORD Lun, DWORD Channel) {
    char deviceName[256] = { 0 };

    if (snprintf(deviceName, sizeof(deviceName), "/dev/pcsc/%lu", Channel) < 0) {
        return IFD_COMMUNICATION_ERROR;
    }

    return IFDHCreateChannelByName(Lun, deviceName);
}

RESPONSECODE IFDHCloseChannel(DWORD Lun) {
    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        return IFD_COMMUNICATION_ERROR;
    }

    reader_status_t r = reader_power_off(reader);
    (void)r; // TODO: log r on error

    r = reader_close(reader);
    (void)r; // TODO: log r on error

    reader_list_free_reader(Lun);

    return IFD_SUCCESS;
}

RESPONSECODE GetCapability(DWORD Length, const UCHAR* value, PDWORD targetLength, PUCHAR targetValue) {
    if (!targetLength) {
        return IFD_COMMUNICATION_ERROR;
    }

    if (*targetLength < Length) {
        *targetLength = Length;
        return IFD_ERROR_INSUFFICIENT_BUFFER;
    }

    *targetLength = Length;

    if (!targetValue) {
        return IFD_SUCCESS;
    }

    memcpy(targetValue, value, Length);
    return IFD_SUCCESS;
}

RESPONSECODE IFDHGetCapabilities(DWORD Lun, DWORD Tag, PDWORD Length, PUCHAR Value) {
    switch (Tag) {
    case TAG_IFD_ATR:
    case SCARD_ATTR_ATR_STRING: {
        Reader* reader = reader_list_get_reader(Lun);
        if (!reader) {
            return IFD_COMMUNICATION_ERROR;
        }

        const UCHAR* atr;
        DWORD atrLen;
        reader_status_t r = reader_get_atr(reader, &atr, &atrLen);
        if (r != reader_status_ok) {
            return IFD_COMMUNICATION_ERROR;
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
        return IFD_NOT_SUPPORTED;
    default:
        return IFD_ERROR_TAG;
    }
}

RESPONSECODE IFDHSetCapabilities(DWORD Lun, DWORD Tag, DWORD Length, PUCHAR Value) {
    return IFD_NOT_SUPPORTED;
}

RESPONSECODE IFDHSetProtocolParameters(DWORD Lun, DWORD Protocol, UCHAR Flags, UCHAR PTS1, UCHAR PTS2, UCHAR PTS3) {
    return IFD_NOT_SUPPORTED;
}

RESPONSECODE IFDHPowerICC(DWORD Lun, DWORD Action, PUCHAR Atr, PDWORD AtrLength) {
    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        return IFD_COMMUNICATION_ERROR;
    }

    reader_status_t r;

    switch (Action) {
    case IFD_POWER_UP:
    case IFD_RESET: {
        r = reader_power_on(reader);
        if (r != reader_status_ok) {
            return IFD_COMMUNICATION_ERROR;
        }

        const UCHAR* atr;
        DWORD atrLen;
        r = reader_get_atr(reader, &atr, &atrLen);
        if (r != reader_status_ok) {
            return IFD_COMMUNICATION_ERROR;
        }

        return GetCapability(atrLen, atr, AtrLength, Atr);
    }
    case IFD_POWER_DOWN:
        r = reader_power_off(reader);
        if (r != reader_status_ok) {
            return IFD_COMMUNICATION_ERROR;
        }
        return IFD_SUCCESS;
    default:
        return IFD_ERROR_NOT_SUPPORTED;
    }
}

RESPONSECODE IFDHTransmitToICC(DWORD Lun, SCARD_IO_HEADER SendPci, PUCHAR TxBuffer, DWORD TxLength, PUCHAR RxBuffer,
                               PDWORD RxLength, PSCARD_IO_HEADER RecvPci) {
    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        return IFD_COMMUNICATION_ERROR;
    }

    reader_status_t r = reader_transmit(reader, TxBuffer, TxLength, RxBuffer, RxLength);
    if (r != reader_status_ok) {
        return IFD_COMMUNICATION_ERROR;
    }

    return IFD_SUCCESS;
}

RESPONSECODE IFDHICCPresence(DWORD Lun) {
    Reader* reader = reader_list_get_reader(Lun);
    if (!reader) {
        return IFD_COMMUNICATION_ERROR;
    }

    reader_status_t r = reader_is_present(reader);
    if (r == reader_status_reader_not_found) {
        return IFD_ICC_NOT_PRESENT;
    } else if (r != reader_status_ok) {
        return IFD_COMMUNICATION_ERROR;
    }

    return IFD_SUCCESS;
}