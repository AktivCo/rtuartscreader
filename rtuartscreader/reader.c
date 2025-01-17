// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/reader.h>

#include <stdint.h>
#include <string.h>

#include <log/log.h>

#include <rtuartscreader/iso7816_3/apdu_t0.h>
#include <rtuartscreader/reader_detail.h>
#include <rtuartscreader/transport/initialize.h>
#include <rtuartscreader/transport/reset.h>
#include <rtuartscreader/transport/sendrecv.h>
#include <rtuartscreader/utils/common.h>
#include <rtuartscreader/utils/error.h>

reader_status_t reader_open(Reader* reader, const char* readerName) {
    transport_status_t r = transport_initialize(&reader->transport, readerName);
    POPULATE_ERROR(r, transport_status_ok, reader_status_internal_error);

    return reader_status_ok;
}

reader_status_t reader_close(Reader* reader) {
    transport_status_t r = transport_deinitialize(&reader->transport);
    POPULATE_ERROR(r, transport_status_ok, reader_status_internal_error);

    return reader_status_ok;
}

reader_status_t reader_get_atr(Reader const* reader, UCHAR const** atr, DWORD* length) {
    *atr = reader->atr;
    *length = reader->atrLength;

    return reader_status_ok;
}

static reader_status_t reader_reset_impl(Reader* reader) {
    size_t atrLength;
    transport_status_t r = transport_reset(&reader->transport, reader->atr, &atrLength);
    POPULATE_ERROR(r, transport_status_ok, reader_status_internal_error);
    reader->atrLength = atrLength;

    return reader_status_ok;
}

reader_status_t reader_power_off(Reader* reader) {
    if (reader->power == POWERED_OFF) {
        return reader_status_ok;
    }

    reader_status_t r = reader_reset_impl(reader);
    if (r != reader_status_ok) {
        return r;
    }

    reader->power = POWERED_OFF;

    return reader_status_ok;
}

reader_status_t reader_power_on(Reader* reader, UCHAR const** atr, DWORD* length) {
    return reader_reset(reader, atr, length);
}

reader_status_t reader_reset(Reader* reader, UCHAR const** atr, DWORD* length) {
    reader_status_t r = reader_reset_impl(reader);
    if (r != reader_status_ok) {
        reader->power = POWERED_OFF;
        return r;
    }

    reader->power = POWERED_ON;

    return reader_get_atr(reader, atr, length);
}

reader_status_t reader_transmit(Reader* reader, UCHAR const* txBuffer, DWORD txLength, UCHAR* rxBuffer, PDWORD rxLength) {
    iso7816_3_status_t r = iso7816_3_status_ok;

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

    if (r == iso7816_3_status_ok)
        *rxLength = recvLength;
    else
        *rxLength = 0;

    // TODO: figure out whether to reset the card in case of communication error
    if (r == iso7816_3_status_communication_error) {
        return reader_status_communication_error;
    } else if (r == iso7816_3_status_insufficient_buffer) {
        return reader_status_memory_error;
    } else if (r != iso7816_3_status_ok) {
        return reader_status_internal_error;
    }

    return reader_status_ok;
}

reader_status_t reader_is_present(Reader* reader) {
    if (reader->presence == PRESENT_FALSE || reader->power == POWERED_OFF) {
        reader->presence = PRESENT_FALSE;
        reader_status_t r = reader_reset_impl(reader);
        POPULATE_ERROR(r, reader_status_ok, reader_status_reader_not_found);
    } else {
        // TODO: FIX ME: Can not transmit APDU to check card presence, because it may break
        // communication performed by upstack application with the card, if the presence
        // check call occures when card expects GET DATA APDU in response to 61XX SW.
    }

    reader->presence = PRESENT_TRUE;
    return reader_status_ok;
}

reader_status_t reader_is_powered(const Reader* reader) {
    return reader->power == POWERED_ON ? reader_status_ok : reader_status_reader_unpowered;
}
