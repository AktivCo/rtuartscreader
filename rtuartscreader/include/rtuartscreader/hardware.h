#pragma once

#include <stdint.h>

typedef enum {
    hw_status_ok = 0,
    hw_status_failed
} hw_status_t;

hw_status_t hw_initialize();

hw_status_t hw_start_clock(uint32_t frequency);

hw_status_t hw_stop_clock();

hw_status_t hw_rst_initialize();

hw_status_t hw_rst_down_up(uint32_t delay_us);

hw_status_t hw_rst_deinitialize();

void hw_deinitialize();
