// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#define NTH_BIT_ONLY(x, bit_number) ((x) & (1ul << (bit_number - 1)))

#define HIOCT(x) ((x & 0xf0) >> 4)
#define LOWOCT(x) (x & 0x0f)

#define BAD_ATR_OFFSET 0xFF