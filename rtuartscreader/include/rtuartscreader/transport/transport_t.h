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
