// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/transport/reset.h>

#include <math.h>
#include <stdbool.h>
#include <string.h>

#include <rtuartscreader/hardware/hardware.h>
#include <rtuartscreader/iso7816_3/atr.h>
#include <rtuartscreader/iso7816_3/pps.h>
#include <rtuartscreader/iso7816_3/utils.h>
#include <rtuartscreader/transport/detail/error.h>
#include <rtuartscreader/transport/detail/transmit_params.h>
#include <rtuartscreader/transport/initialize.h>
#include <rtuartscreader/utils/common.h>

typedef struct {
    speed_t baudrate;
    double symbol_time_us;
} transport_params_entry_t;

#define BAUDRATE_AND_SYMBOL_TIME_US_PAIR(n) \
    { B##n, (S_TO_US_MULTIPLIER_LF / n) }

static int transmit_speed_from_f_d(uint32_t f, uint32_t d, uint32_t max_freq, transmit_speed_t* transmit_speed) {
    static const transport_params_entry_t default_values[16] = {
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(75),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(110),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(134),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(150),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(300),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(600),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(1200),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(1800),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(2400),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(4800),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(9600),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(19200),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(38400),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(57600),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(115200),
        BAUDRATE_AND_SYMBOL_TIME_US_PAIR(230400),
    };

    static const uint32_t min_freq = 1e6;

    uint32_t etu_periods = f / d;

    for (size_t i = ARRAYSIZE(default_values); i != 0; --i) {
        // f = 1 / T; T = symbol_time / etu_periods; symbol_time = symbol_time_us / S_TO_US_MULTIPLIER_LF
        double freq = S_TO_US_MULTIPLIER_LF * etu_periods / (default_values[i - 1].symbol_time_us);
        if (freq >= min_freq && freq < max_freq) {
            transmit_speed->baudrate = default_values[i - 1].baudrate;
            transmit_speed->freq = (uint32_t)freq;
            return 1;
        }
    }

    return 0;
}

#undef BAUDRATE_AND_SYMBOL_TIME_US_PAIR

static int transmit_speed_from_f_d_indices(const f_d_index_t* f_d_index, transmit_speed_t* transmit_speed) {
    const f_freq_max_t* f_freq_max = f_freq_max_by_index(f_d_index->f_index);
    uint32_t d = d_by_index(f_d_index->d_index);

    if (f_freq_max->f == IS0_7816_3_RFU || f_freq_max->freq_max_hz == IS0_7816_3_RFU || d == IS0_7816_3_RFU) {
        return 0;
    }

    return transmit_speed_from_f_d(f_freq_max->f, d, f_freq_max->freq_max_hz, transmit_speed);
}

// left is worse than right
static inline bool transmit_speed_is_worse_than(const transmit_speed_t* left, const transmit_speed_t* right) {
    return (left->baudrate < right->baudrate) //
           || (left->baudrate == right->baudrate && left->freq > right->freq);
}

static int choose_best_f_d_indices(const f_d_index_t* f_d_index_max, f_d_index_t* f_d_index_result) {
    bool found_first = false;
    transmit_speed_t transmit_speed_best;

    for (size_t f_index = 0; f_index <= f_d_index_max->f_index; ++f_index) {
        for (size_t d_index = 0; d_index <= f_d_index_max->d_index; ++d_index) {
            f_d_index_t f_d_index_tested = { .f_index = f_index, .d_index = d_index };
            transmit_speed_t transmit_speed_tested;

            if (!transmit_speed_from_f_d_indices(&f_d_index_tested, &transmit_speed_tested)) continue;

            if (!found_first) {
                transmit_speed_best = transmit_speed_tested;
                *f_d_index_result = f_d_index_tested;

                found_first = true;

                continue;
            }

            if (transmit_speed_is_worse_than(&transmit_speed_best, &transmit_speed_tested)) {
                transmit_speed_best = transmit_speed_tested;
                *f_d_index_result = f_d_index_tested;
            }
        }
    }

    return found_first;
}

static transport_status_t compute_wt_ds(const atr_info_t* atr_info, uint32_t freq, uint8_t* wt_ds) {
    double wt;
    iso7816_3_status_t r = compute_wt(atr_info, freq, &wt);
    POPULATE_ERROR(r, iso7816_3_status_ok, transport_status_invalid_atr);

    double s_to_ds_multiplier = 10;
    double wt_ds_claimed = s_to_ds_multiplier * wt;
    if (wt_ds_claimed > 255) {
        // TODO: log the problem
        // TODO: support insanely long wait time
        LOG_RETURN_TRANSPORT_ERROR_MSG(transport_status_mode_not_supported,
                                       "Card transmission parameters (too long WT) is not supported");
    }

    *wt_ds = (uint8_t)(ceil(wt_ds_claimed));

    return transport_status_ok;
}

static transport_status_t transmit_params_init(const f_d_index_t* f_d_index, const atr_info_t* atr_info,
                                               transmit_params_t* params) {
    uint32_t f = f_freq_max_by_index(f_d_index->f_index)->f;
    uint32_t d = d_by_index(f_d_index->d_index);

    params->etu = f / d;

    if (!transmit_speed_from_f_d_indices(f_d_index, &params->transmit_speed)) {
        LOG_RETURN_TRANSPORT_ERROR_MSG(transport_status_mode_not_supported,
                                       "Card transmission parameters (F, D) are not supported");
    };

    iso7816_3_status_t iso_r = compute_extra_gt(f, d, atr_info, params->transmit_speed.freq, &params->extra_gt_us);
    POPULATE_ERROR(iso_r, iso7816_3_status_ok, transport_status_invalid_atr);

    transport_status_t r = compute_wt_ds(atr_info, params->transmit_speed.freq, &params->wt_ds);
    POPULATE_ERROR(r, transport_status_ok, r);

    return transport_status_ok;
}

static uint32_t calculate_reset_us_delay(uint32_t freq) {
    uint32_t us_delay_default = 400. / transmit_params_default()->transmit_speed.freq * S_TO_US_MULTIPLIER_LF + 1;

    uint32_t us_delay = 400. / freq * S_TO_US_MULTIPLIER_LF + 1;

    return us_delay > us_delay_default ? us_delay : us_delay_default;
}

static bool is_explicit_protocol_defined(bool* explicit_protocols, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (explicit_protocols[i]) return true;
    }

    return false;
}

static transport_status_t do_transport_reset(transport_t* transport, uint8_t atr_buffer[], size_t* atr_len) {
    uint32_t delay_us = calculate_reset_us_delay(transport->params.transmit_speed.freq);

    hw_status_t hw_r = hw_rst_down();
    RETURN_ON_HW_ERROR(hw_r);

    transport_status_t r = transport_reinitialize(transport, transmit_params_default());
    POPULATE_ERROR(r, transport_status_ok, r);

    hw_r = hw_rst_down_up(delay_us);
    RETURN_ON_HW_ERROR(hw_r);

    atr_t atr;
    iso7816_3_status_t iso_r = read_atr(transport, &atr);
    RETURN_ON_IS07816_3_ERROR(iso_r);

    atr_info_t info;
    iso_r = parse_atr(&atr, &info);
    RETURN_ON_IS07816_3_ERROR(iso_r);

    memcpy(atr_buffer, atr.atr, atr.atr_len);
    *atr_len = atr.atr_len;

    // Choose protocol: T0 only is supported
    uint8_t protocol = PROTOCOL_T0;
    if (info.ta2.is_present) {
        if (info.ta2.enforced_protocol != PROTOCOL_T0) {
            if (info.ta2.can_change_mode) {
                LOG_RETURN_TRANSPORT_ERROR_MSG(transport_status_need_reset, "T0 only is supported, may try again");
            } else {
                LOG_RETURN_TRANSPORT_ERROR_MSG(transport_status_mode_not_supported, "T0 only is supported");
            }
        }
    } else {
        if (is_explicit_protocol_defined(info.explicit_protocols, ARRAYSIZE(info.explicit_protocols)) //
            && !info.explicit_protocols[PROTOCOL_T0]) {
            LOG_RETURN_TRANSPORT_ERROR_MSG(transport_status_mode_not_supported, "T0 only is supported");
        }
    }

    // Choose F & D
    f_d_index_t f_d_index = f_d_index_default;

    if (info.ta1.is_present && !choose_best_f_d_indices(&info.ta1.f_d, &f_d_index)) {
        LOG_RETURN_TRANSPORT_ERROR_MSG(transport_status_mode_not_supported,
                                       "Card transmission parameters (F, D) are not supported");
    }

    iso_r = do_pps_exchange(transport, &f_d_index, protocol);
    if (iso_r != iso7816_3_status_ok)
    {
        if (iso_r == iso7816_3_status_pps_exchange_use_default_f_d)
            f_d_index = f_d_index_default;
        else
            RETURN_ON_IS07816_3_ERROR(iso_r);
    }

    // Assert F & D are OK
    transmit_params_t params;
    r = transmit_params_init(&f_d_index, &info, &params);
    POPULATE_ERROR(r, transport_status_ok, r);

    r = transport_reinitialize(transport, &params);
    POPULATE_ERROR(r, transport_status_ok, r);

    return transport_status_ok;
}

transport_status_t transport_reset(transport_t* transport, uint8_t atr_buffer[], size_t* atr_len) {
    transport_status_t r = do_transport_reset(transport, atr_buffer, atr_len);
    if (r == transport_status_need_reset) {
        // TODO: may there be more iterations?
        r = do_transport_reset(transport, atr_buffer, atr_len);
    }

    return r;
}
