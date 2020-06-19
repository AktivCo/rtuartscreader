#include <rtuartscreader/hardware.h>

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

hw_status_t hw_initialize() {
    if (gpioInitialise() == PI_INIT_FAILED)
        return hw_status_failed;

    return hw_status_ok;
}

hw_status_t hw_start_clock(uint32_t frequency) {
    int r = gpioSetMode(PIN_PWM0, PI_ALT5);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioHardwarePWM(PIN_PWM0, frequency, DUTY_CYCLE_50_PERCENT);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_stop_clock() {
    int r = stopHardwarePWM();
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_initialize() {
    int r = gpioSetMode(PIN_SC_RST, PI_OUTPUT);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_down() {
    int r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_down_up(uint32_t delay_us) {
    int r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioSleep(PI_TIME_RELATIVE, 0, delay_us);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioWrite(PIN_SC_RST, 1);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

hw_status_t hw_rst_deinitialize() {
    int r = gpioWrite(PIN_SC_RST, 0);
    RETURN_ON_PIGPIO_ERROR(r);

    r = gpioSetMode(PIN_SC_RST, PI_INPUT);
    RETURN_ON_PIGPIO_ERROR(r);

    return hw_status_ok;
}

void hw_deinitialize() {
    gpioTerminate();
}
