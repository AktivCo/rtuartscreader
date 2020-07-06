// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <fakehardware/fakehardware.h>

#include <rtuartscreader/hardware/hardware.h>

hw_status_t hw_initialize_impl() {
    return hw_status_ok;
}

hw_status_t hw_start_clock_impl(uint32_t frequency) {
    return hw_status_ok;
}

hw_status_t hw_stop_clock_impl() {
    return hw_status_ok;
}

hw_status_t hw_rst_initialize_impl() {
    return hw_status_ok;
}

hw_status_t hw_rst_down_impl() {
    return hw_status_ok;
}

hw_status_t hw_rst_down_up_impl(uint32_t delay_us) {
    return hw_status_ok;
}

hw_status_t hw_rst_deinitialize_impl() {
    return hw_status_ok;
}

void hw_deinitialize_impl() {
}

hw_impl_t gHardwareImpl = {
    .hw_initialize = hw_initialize_impl,
    .hw_start_clock = hw_start_clock_impl,
    .hw_stop_clock = hw_stop_clock_impl,
    .hw_rst_initialize = hw_rst_initialize_impl,
    .hw_rst_down = hw_rst_down_impl,
    .hw_rst_down_up = hw_rst_down_up_impl,
    .hw_rst_deinitialize = hw_rst_deinitialize_impl,
    .hw_deinitialize = hw_deinitialize_impl
};

namespace rt {
namespace fakehardware {

void initialize() {
    hw_impl_set(&gHardwareImpl);
}
void deinitialize() {
    hw_impl_reset();
}

} // namespace fakehardware
} // namespace rt