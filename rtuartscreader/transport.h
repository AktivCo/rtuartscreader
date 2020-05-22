#pragma once

#include <stddef.h>

#include <stdint.h>

typedef struct {
    uint32_t self;
    double max_freq;
    uint8_t WT;
} etu_t;

typedef struct {
    int handle;
    uint32_t baudrate;
    uint32_t freq;
    etu_t etu;
} transport_t;

typedef enum {
    transport_status_ok = 0,
    transport_status_timeout,
    transport_status_communication_error,
    transport_status_os_error,
    transport_status_hardware_error
} transport_status_t;


transport_status_t transport_initialize(transport_t* transport, const char* reader_name);

transport_status_t transport_deinitialize(const transport_t* transport);

transport_status_t transport_recv_byte(const transport_t* transport, uint8_t* byte);

transport_status_t transport_send_byte(const transport_t* transport, uint8_t byte);

transport_status_t transport_recv_bytes(const transport_t* transport, uint8_t* buf, size_t len);

transport_status_t transport_send_bytes(const transport_t* transport, const uint8_t* bytes, size_t len);
