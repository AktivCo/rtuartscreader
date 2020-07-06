// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <rtuartscreader/iso7816_3/atr.h>
#include <rtuartscreader/iso7816_3/status.h>
#include <stdint.h>

#define IS0_7816_3_RFU 0

#define S_TO_US_MULTIPLIER_LF ((double)1e6)

typedef struct f_freq_max {
    uint32_t f;
    uint32_t freq_max_hz;
} f_freq_max_t;

const f_freq_max_t* f_freq_max_by_index(uint8_t index);

uint32_t d_by_index(uint8_t index);

iso7816_3_status_t compute_extra_gt(uint32_t f, uint32_t d, const atr_info_t* atr_info, uint32_t freq,
                                    uint32_t* extra_gt_us);

iso7816_3_status_t compute_wt(const atr_info_t* atr_info, uint32_t freq, double* wt);