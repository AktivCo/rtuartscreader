// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "reader.h"

#include <stdbool.h>
#include <string.h>

#include <termios.h>

#include "reader_detail.h"

const UCHAR kEcpBAtr[] = { 0X3B, 0X8B, 0X01, 0X52, 0X75, 0X74, 0X6F, 0X6B, 0X65, 0X6E, 0X20, 0X44, 0X53, 0X42, 0XA3 };
const UCHAR kEcp20Atr[] = { 0x3B, 0x8B, 0x01, 0x52, 0x75, 0x74, 0x6F, 0x6B, 0x65, 0x6E, 0x20, 0x44, 0x53, 0x20, 0xC1 };

reader_status_t reader_status(io_status_t error) {
    switch (error) {
    case IO_ERROR__OK: return reader_status_ok;
    case IO_ERROR__INVALID_ARG: return reader_status_internal_error;
    case IO_ERROR__NO_MEMORY: return reader_status_memory_error;
    case IO_ERROR__INTERNAL: return reader_status_internal_error;
    case IO_ERROR__PROTOCOL:
    case IO_ERROR__CRC:
    case IO_ERROR__PACKET_LEN:
    case IO_ERROR__DATA_LEN:
    case IO_ERROR__PACKET_NUM:
    case IO_ERROR__PACKET_NUM_PREV:
    case IO_ERROR__INVALID_HDR_INFO:
    case IO_ERROR__INVALID_PROTOCOL_NUM:
    case IO_ERROR__WAIT:
    case IO_ERROR__INVALID_COMMAND:
    case IO_ERROR__TIMEOUT:
    case IO_ERROR__PAIR: return reader_status_communication_error;
    case IO_ERROR__NO_SUCH_DEVICE: return reader_status_reader_not_found;
    case IO_ERROR__CLOSE: return reader_status_communication_error;
    case IO_ERROR__UNSUCCESSFUL: return reader_status_internal_error;
    }
    return reader_status_internal_error;
}

reader_status_t reader_reset(Reader* reader);
reader_status_t reader_apdu_reset(Reader* reader);
reader_status_t reader_hardware_reset(Reader* reader);

bool is_rutoken_ecp_b_atr(const UCHAR* atr, DWORD atrLength) {
    return (sizeof(kEcpBAtr) == atrLength) && !memcmp(kEcpBAtr, atr, atrLength);
}

reader_status_t reader_open(Reader* reader, const char* readerName) {
    io_status_t r;

    r = TrAPI_OpenDevice(&(reader->handle), readerName, B115200);
    if (r != IO_ERROR__OK)
        return reader_status(r);

    r = TrAPI_Synchronise(&(reader->handle));
    if (r != IO_ERROR__OK) {
        reader_close(reader);
        return reader_status(r);
    }

    reader_status_t r1 = reader_power_on(reader);
    if (r1 != reader_status_ok) {
        reader_close(reader);
        return r1;
    }

    return reader_power_off(reader);
}

reader_status_t reader_close(Reader* reader) {
    io_status_t r;

    r = TrAPI_CloseDevice(&(reader->handle));

    return reader_status(r);
}

reader_status_t reader_get_atr(Reader const* reader, UCHAR const** atr, DWORD* length) {
    *atr = reader->atr;
    *length = reader->atrLength;

    return reader_status_ok;
}

reader_status_t reader_power_off(Reader* reader) {
    if (reader->power == POWERED_ON) {
        (void)reader_reset(reader);
    }

    reader->power = POWERED_OFF;
    memset(reader->atr, 0, sizeof(reader->atr));
    reader->atrLength = 0;

    return reader_status_ok;
}

reader_status_t reader_power_on(Reader* reader) {
    reader_status_t r;

    reader->power = POWERED_ON;

    r = reader_reset(reader);
    if (r != reader_status_ok) {
        reader->power = POWERED_OFF;
    }

    return r;
}

reader_status_t reader_reset(Reader* reader) {
    reader_status_t r = reader_apdu_reset(reader);
    if (r == reader_status_ok) {
        return r;
    }

    // Worst case: smartcard is in chain command
    r = reader_hardware_reset(reader);
    if (r != reader_status_ok) {
        return r;
    }

    return reader_apdu_reset(reader);
}

reader_status_t reader_hardware_reset(Reader* reader) {
    io_status_t r;

    r = TrAPI_Reset(&(reader->handle));

    return reader_status(r);
}

reader_status_t reader_apdu_reset(Reader* reader) {
    UCHAR txBuffer[] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F };
    UCHAR rxBuffer[0x0F + 2] = { 0 };
    DWORD rxLength = sizeof(rxBuffer);

    reader_status_t r = reader_transmit(reader, txBuffer, sizeof(txBuffer), rxBuffer, &rxLength);
    if (r != reader_status_ok)
        return r;

    if (rxLength < 2 || rxBuffer[rxLength - 1] != 0x00 || rxBuffer[rxLength - 2] != 0x90)
        return reader_status_communication_error;

    if (!is_rutoken_ecp_b_atr(rxBuffer, rxLength - 2)) {
        return reader_status_reader_not_found;
    }

    // Replace original ATR with fake one, that is supported by rtPKCS11ECP
    reader->atrLength = sizeof(kEcp20Atr);
    memcpy(reader->atr, kEcp20Atr, sizeof(kEcp20Atr));

    return reader_status_ok;
}

reader_status_t reader_transmit(Reader* reader, UCHAR const* txBuffer, DWORD txLength, UCHAR* rxBuffer, PDWORD rxLength) {
    if (reader->power != POWERED_ON) {
        return reader_status_reader_unpowered;
    }

    uint16_t sendLength;
    uint16_t recvLength;

    if (txLength > UINT16_MAX)
        return reader_status_internal_error;
    else
        sendLength = txLength;

    if (*rxLength > UINT16_MAX)
        recvLength = UINT16_MAX;
    else
        recvLength = *rxLength;

    io_status_t r = TrAPI_Transmit(&(reader->handle), txBuffer, sendLength, rxBuffer, &recvLength);
    if (r != IO_ERROR__OK)
        return reader_status(r);

    *rxLength = recvLength;
    return reader_status_ok;
}

reader_status_t reader_is_present(Reader* reader) {
    io_status_t r;

    r = TrAPI_Synchronise(&(reader->handle));
    if (r == IO_ERROR__TIMEOUT) {
        return reader_status_reader_not_found;
    } else {
        return reader_status(r);
    }
}
