// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "reader.h"

#include <stdint.h>
#include <string.h>

#include "apdu_t0.h"
#include "reader_detail.h"
#include "transport.h"


// TODO: remove me
static const uint8_t kFakeAtr[] = { 0x3B, 0x1A, 0x96, 0x72, 0x75, 0x74, 0x6F, 0x6B, 0x65, 0x6E, 0x6D, 0x73, 0x63 };
static const size_t kFakeAtrLen = sizeof(kFakeAtr);

reader_status_t reader_open(Reader* reader, const char* readerName) {
    transport_status_t r = transport_initialize(&reader->transport, readerName);
    if (r)
        return reader_status_internal_error;

    return reader_status_ok;
}

reader_status_t reader_close(Reader* reader) {
    transport_status_t r = transport_deinitialize(&reader->transport);
    if (r)
        return reader_status_internal_error;

    return reader_status_ok;
}

reader_status_t reader_get_atr(Reader const* reader, UCHAR const** atr, DWORD* length) {
    *atr = reader->atr;
    *length = reader->atrLength;

    return reader_status_ok;
}

reader_status_t reader_power_off_impl(Reader* reader) {
    // TODO

    memset(reader->atr, 0, sizeof(reader->atr));
    reader->atrLength = 0;

    return reader_status_ok;
}

reader_status_t reader_power_off(Reader* reader) {
    reader_status_t r = reader_status_ok;

    if (reader->power == POWERED_ON) {
        r = reader_power_off_impl(reader);
    }

    if (r != reader_status_ok) {
        return r;
    }

    reader->power = POWERED_OFF;

    return reader_status_ok;
}

reader_status_t reader_power_on_impl(Reader* reader) {
    // TODO: power on and fill reader->atr and reader->atrLength
    // TODO: return IFD_COMMUNICATION_ERROR if power on is failed due to unresponsive/absent card
    // TODO: set reader->atrLength to 0 and return IFD_ERROR_POWER_ACTION if power on is failed for other reasons

    // TODO: remove me
    memcpy(reader->atr, kFakeAtr, kFakeAtrLen);
    reader->atrLength = kFakeAtrLen;

    return reader_status_ok;
}

reader_status_t reader_power_on(Reader* reader, UCHAR const** atr, DWORD* length) {
    reader_status_t r;

    reader->power = POWERED_ON;

    r = reader_power_on_impl(reader);
    if (r != reader_status_ok) {
        reader->power = POWERED_OFF;
        return r;
    }

    return reader_get_atr(reader, atr, length);
}

reader_status_t reader_reset_impl(Reader* reader) {
    // TODO: reset reader and fill reader->atr and reader->atrLength
    // TODO: return IFD_COMMUNICATION_ERROR if reset is failed due to unresponsive/absent card
    // TODO: set reader->atrLength to 0 and return IFD_ERROR_POWER_ACTION if reset is failed for other reasons

    // TODO: remove me
    memcpy(reader->atr, kFakeAtr, kFakeAtrLen);
    reader->atrLength = kFakeAtrLen;

    return reader_status_ok;
}

reader_status_t reader_reset(Reader* reader, UCHAR const** atr, DWORD* length) {
    reader_status_t r;

    if (reader->power == POWERED_OFF) {
        r = reader_power_on_impl(reader);

        if (r != reader_status_ok) {
            return r;
        }

        reader->power = POWERED_ON;
    }

    r = reader_reset_impl(reader);

    if (r != reader_status_ok) {
        return r;
    }

    return reader_get_atr(reader, atr, length);
}

reader_status_t reader_transmit(Reader* reader, UCHAR const* txBuffer, DWORD txLength, UCHAR* rxBuffer, PDWORD rxLength) {
    transmit_status_t r = transmit_status_ok;

    if (reader->power != POWERED_ON) {
        return reader_status_reader_unpowered;
    }

    uint16_t sendLength;
    uint16_t recvLength;

    if ((txBuffer == NULL && txLength != 0) || rxLength == NULL) {
        LOG_ERROR("reader_transmit failed: wrong args");

        return reader_status_internal_error;
    }

    if (txLength > UINT16_MAX)
        return reader_status_internal_error;
    else
        sendLength = txLength;

    if (*rxLength > UINT16_MAX)
        recvLength = UINT16_MAX;
    else
        recvLength = *rxLength;

    r = t0_transmit_apdu(&reader->transport, txBuffer, sendLength, rxBuffer, &recvLength);

    if (r == transmit_status_ok)
        *rxLength = recvLength;
    else
        *rxLength = 0;

    // TODO: figure out whether to reset the card in case of communication error
    if (r == transmit_status_communication_error) {
        return reader_status_communication_error;
    } else if (r == transmit_status_insufficient_buffer) {
        return reader_status_memory_error;
    } else if (r != transmit_status_ok) {
        return reader_status_internal_error;
    }

    return reader_status_ok;
}

reader_status_t reader_is_present(Reader* reader) {
    reader_status_t r = reader_status_ok;

    // TODO: check presence and return either reader_status_reader_not_found or reader_status_ok

    return r;
}
