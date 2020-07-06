// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/transport/detail/transmit_params.h>

#include <math.h>

#define ETU_372_FREQUENCY_HZ 3570058
#define DEFAULT_ETU 372

const transmit_params_t* transmit_params_default() {
    static const transmit_params_t transmit_params = {
        .transmit_speed = {
            .freq = ETU_372_FREQUENCY_HZ,
            .baudrate = B9600 },
        .etu = DEFAULT_ETU,
        .extra_gt_us = 0,
        .wt_ds = (uint8_t)(10 * 9600. * DEFAULT_ETU / ETU_372_FREQUENCY_HZ + 1)
    };

    return &transmit_params;
}
