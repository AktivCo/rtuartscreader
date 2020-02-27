#pragma once

#include <stdint.h>

#include <TransportAPI/types.h>

#ifdef __cplusplus
extern "C" {
#endif

io_status_t com_init(HANDLE *hcom, const char *com_name, uint32_t baudrate);

io_status_t com_deinit(HANDLE hcom);

#ifdef __cplusplus
}
#endif
