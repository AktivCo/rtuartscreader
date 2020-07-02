#include <rtuartscreader/hardware/hardware.h>

hw_status_t hw_initialize_impl() {
    return hw_status_failed;
}

hw_status_t hw_start_clock_impl(uint32_t frequency) {
    return hw_status_failed;
}

hw_status_t hw_stop_clock_impl() {
    return hw_status_failed;
}

hw_status_t hw_rst_initialize_impl() {
    return hw_status_failed;
}

hw_status_t hw_rst_down_impl() {
    return hw_status_failed;
}

hw_status_t hw_rst_down_up_impl(uint32_t delay_us) {
    return hw_status_failed;
}

hw_status_t hw_rst_deinitialize_impl() {
    return hw_status_failed;
}

void hw_deinitialize_impl() {
}

#define PIMPL_NAME_PREFIX hw
#define PIMPL_FUNCTIONS_DECLARATION_PATH <rtuartscreader/hardware/detail/hardware_functions.h>
#include <rtuartscreader/pimpl/source.h>