#pragma once

#include <PCSC/ifdhandler.h>

typedef struct reader_st Reader;

typedef enum {
    reader_status_ok = 0,
    reader_status_reader_not_found,
    reader_status_reader_unpowered,
    reader_status_memory_error,
    reader_status_communication_error,
    reader_status_internal_error
} reader_status_t;

reader_status_t reader_open(Reader* reader, const char* readerName);
reader_status_t reader_close(Reader* reader);
reader_status_t reader_get_atr(Reader const* reader, UCHAR const** atr, DWORD* length);
reader_status_t reader_power_off(Reader* reader);
reader_status_t reader_power_on(Reader* reader);
reader_status_t reader_reset(Reader* reader);
reader_status_t reader_transmit(Reader* reader, UCHAR const* txBuffer, DWORD txLength, UCHAR* rxBuffer, PDWORD rxLength);
reader_status_t reader_is_present(Reader* reader);
