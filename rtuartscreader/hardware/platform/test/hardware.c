#include <rtuartscreader/hardware.h>

hw_status_t hw_initialize() {
    return hw_status_ok;
}

hw_status_t hw_start_clock(uint32_t frequency) {
    return hw_status_ok;
}

hw_status_t hw_stop_clock() {
    return hw_status_ok;
}

hw_status_t hw_rst_initialize() {
    return hw_status_ok;
}

hw_status_t hw_rst_down_up(uint32_t delay_us) {
    return hw_status_ok;
}

hw_status_t hw_rst_deinitialize() {
    return hw_status_ok;
}

void hw_deinitialize() {
}
