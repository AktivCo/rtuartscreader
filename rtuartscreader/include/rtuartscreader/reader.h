// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <PCSC/ifdhandler.h>

#include "log.h"

typedef struct reader_st Reader;

typedef enum {
    reader_status_ok = 0,
    reader_status_reader_not_found,
    reader_status_reader_unpowered,
    reader_status_memory_error,
    reader_status_communication_error,
    reader_status_internal_error,
    reader_status_timeout
} reader_status_t;

reader_status_t reader_open(Reader* reader, const char* readerName);
reader_status_t reader_close(Reader* reader);
reader_status_t reader_get_atr(Reader const* reader, UCHAR const** atr, DWORD* length);
reader_status_t reader_power_off(Reader* reader);
reader_status_t reader_power_on(Reader* reader, UCHAR const** atr, DWORD* length);
reader_status_t reader_reset(Reader* reader, UCHAR const** atr, DWORD* length);
reader_status_t reader_transmit(Reader* reader, UCHAR const* txBuffer, DWORD txLength, UCHAR* rxBuffer, PDWORD rxLength);
reader_status_t reader_is_present(Reader* reader);
reader_status_t reader_is_powered(const Reader* reader);
