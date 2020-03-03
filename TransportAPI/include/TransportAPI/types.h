// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stdint.h>

typedef enum {
    IO_ERROR__OK = 0,                    // Ok
    IO_ERROR__INVALID_ARG = 1,           // Invalid arguments
    IO_ERROR__NO_MEMORY = 2,             //
    IO_ERROR__INTERNAL = 3,              //
    IO_ERROR__PROTOCOL = 4,              // Connection error(send\recv error, invalid data length, ...)
    IO_ERROR__CRC = 5,                   // Invalid received packet's CRC
    IO_ERROR__PACKET_LEN = 6,            // Size of received packet is out of bounds.
    IO_ERROR__DATA_LEN = 7,              // Wrong data length
    IO_ERROR__PACKET_NUM = 8,            // Packet number differs from expected value
    IO_ERROR__PACKET_NUM_PREV = 9,       // Previous packet number(repeated reply packet)
    IO_ERROR__INVALID_HDR_INFO = 10,     // Wrong header info
    IO_ERROR__INVALID_PROTOCOL_NUM = 11, // Protocol version is not supported
    IO_ERROR__WAIT = 12,                 // Wait-packet
    IO_ERROR__INVALID_COMMAND = 13,      // Unknown command
    IO_ERROR__TIMEOUT = 14,              // Timeout
    IO_ERROR__PAIR = 15,                 // Pair error
    IO_ERROR__NO_SUCH_DEVICE = 16,       //
    IO_ERROR__CLOSE = 17,
    IO_ERROR__UNSUCCESSFUL = 18
} io_status_t;

typedef int HANDLE;

typedef struct {
    HANDLE hPort;
    uint16_t wPacketNum;
} DEVICE_HANDLE, *PDEVICE_HANDLE;
