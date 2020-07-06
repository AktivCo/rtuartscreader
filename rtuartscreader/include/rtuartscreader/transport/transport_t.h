// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <termios.h>

typedef struct transmit_speed {
    uint32_t freq;
    speed_t baudrate;
} transmit_speed_t;

typedef struct transmit_params {
    transmit_speed_t transmit_speed;
    uint32_t etu;
    uint32_t extra_gt_us; // excess over 12 etu
    uint8_t wt_ds;        // d for deci-
} transmit_params_t;

typedef struct {
    int handle;
    transmit_params_t params;
} transport_t;
