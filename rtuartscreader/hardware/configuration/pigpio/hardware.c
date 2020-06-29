#include <rtuartscreader/hardware/hardware.h>

#include <pigpio/pigpio.h>

#define PIN_PWM0 18

#define PIN_SC_RST 17

#define DUTY_CYCLE_50_PERCENT 500000

#define RETURN_ON_PIGPIO_ERROR(r)    \
    do {                             \
        if (r != 0) {                \
            return hw_status_failed; \
        }                            \
    } while (0)

hw_status_t hw_initialize_impl() {
    const uint32_t gpioCfgInternals = PI_CFG_NOSIGHANDLER;
    int r = gpioCfgSetInternals(gpioCfgInternals);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioCfgInterfaces(PI_DISABLE_FIFO_IF | PI_DISABLE_SOCK_IF | PI_DISABLE_ALERT);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioInitialise();
    if (r != PIGPIO_VERSION) {
        return hw_status_failed;
    }

    return hw_status_ok;
}

hw_status_t hw_start_clock_impl(uint32_t frequency) {
    int r = gpioSetMode(PIN_PWM0, PI_ALT5);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioHardwarePWM(PIN_PWM0, frequency, DUTY_CYCLE_50_PERCENT);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_stop_clock_impl() {
    int r = stopHardwarePWM();
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_initialize_impl() {
    int r = gpioSetMode(PIN_SC_RST, PI_OUTPUT);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_down_impl() {
    int r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_down_up_impl(uint32_t delay_us) {
    int r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioSleep(PI_TIME_RELATIVE, 0, delay_us);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioWrite(PIN_SC_RST, 1);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_deinitialize_impl() {
    int r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioSetMode(PIN_SC_RST, PI_INPUT);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

void hw_deinitialize_impl() {
    gpioTerminate();
}

#define PIMPL_NAME_PREFIX hw
#define PIMPL_FUNCTIONS_DECLARATION_PATH <rtuartscreader/hardware/detail/hardware_functions.h>
#include <rtuartscreader/pimpl/source.h>
