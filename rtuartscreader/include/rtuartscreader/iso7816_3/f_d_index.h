// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <stdint.h>

typedef struct f_d_index {
    uint8_t f_index;
    uint8_t d_index;
} f_d_index_t;

extern const f_d_index_t f_d_index_default;