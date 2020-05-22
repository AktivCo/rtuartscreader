#include "transport.h"

#include <fcntl.h>
#include <stddef.h>
#include <termios.h>
#include <unistd.h>

#include "hardware.h"
#include "log.h"

#define DEFAULT_ETU 372
#define DEFAULT_FI DEFAULT_ETU

#define DEFAULT_WI 10

#define DEFAULT_MAX_FREQUENCY_MHZ 5

#define ETU_372_FREQUENCY_HZ 3570058

#define RETURN_ON_HW_ERROR(r)                       \
    do {                                            \
        if (r != hw_status_ok) {                    \
            return transport_status_hardware_error; \
        }                                           \
    } while (0)

#define RETURN_ON_OS_ERROR(r)                 \
    do {                                      \
        if (r != 0) {                         \
            return transport_status_os_error; \
        }                                     \
    } while (0)

transport_status_t transport_setup_serial_settings(const transport_t* transport) {
    struct termios options = { 0 };

    int ret = tcgetattr(transport->handle, &options);
    RETURN_ON_OS_ERROR(ret);

    ret = cfsetispeed(&options, transport->baudrate);
    RETURN_ON_OS_ERROR(ret);

    ret = cfsetospeed(&options, transport->baudrate);
    RETURN_ON_OS_ERROR(ret);

    cfmakeraw(&options);

    options.c_cflag |= CSTOPB | PARENB;
    options.c_cflag &= ~PARODD;

    options.c_cc[VMIN] = 0;

    options.c_cc[VTIME] = transport->etu.WT;

    ret = tcsetattr(transport->handle, TCSANOW, &options);
    RETURN_ON_OS_ERROR(ret);

    ret = tcflush(transport->handle, TCIOFLUSH);
    RETURN_ON_OS_ERROR(ret);

    return transport_status_ok;
}

transport_status_t transport_initialize(transport_t* transport, const char* reader_name) {
    hw_status_t hw_r;
    transport_status_t r;
    int os_r;

    transport->baudrate = B9600;
    transport->freq = ETU_372_FREQUENCY_HZ;
    transport->etu.self = DEFAULT_ETU;
    transport->etu.max_freq = DEFAULT_MAX_FREQUENCY_MHZ;
    transport->etu.WT = (uint8_t)(10 * 960 * DEFAULT_WI * DEFAULT_FI / ETU_372_FREQUENCY_HZ);

    transport->handle = open(reader_name, O_RDWR | O_NOCTTY);
    if (transport->handle == -1) {
        r = transport_status_communication_error;
        goto err_label;
    }

    r = transport_setup_serial_settings(transport);
    if (r != transport_status_ok)
        goto close_handle_label;

    hw_r = hw_initialize();
    if (hw_r != hw_status_ok) {
        r = transport_status_hardware_error;
        goto close_handle_label;
    }

    hw_r = hw_rst_initialize();
    if (hw_r != hw_status_ok) {
        r = transport_status_hardware_error;
        goto deinit_library_label;
    }

    hw_r = hw_start_clock(transport->freq);
    if (hw_r != hw_status_ok) {
        r = transport_status_hardware_error;
        goto deinit_rst_pin_label;
    }

    return transport_status_ok;

deinit_rst_pin_label:
    hw_r = hw_rst_deinitialize();
    RETURN_ON_HW_ERROR(hw_r);
deinit_library_label:
    hw_deinitialize();
close_handle_label:
    os_r = close(transport->handle);
    RETURN_ON_OS_ERROR(os_r);
err_label:
    return r;
}

transport_status_t transport_deinitialize(const transport_t* transport) {
    hw_status_t r = hw_stop_clock();
    RETURN_ON_HW_ERROR(r);

    r = hw_rst_deinitialize();
    RETURN_ON_HW_ERROR(r);

    hw_deinitialize();

    int os_r = close(transport->handle);
    RETURN_ON_OS_ERROR(os_r);

    return transport_status_ok;
}

// Read timeout is expected to be set based on WT value
// derived from selected transport parameters during PPS.
transport_status_t transport_recv_byte(const transport_t* transport, uint8_t* byte) {
    ssize_t rsize = read(transport->handle, byte, 1);

    if (rsize == -1) {
        return transport_status_communication_error;
    }

    if (!rsize) {
        return transport_status_timeout;
    }

    return transport_status_ok;
}

// TODO: support extra guard time
transport_status_t transport_send_byte(const transport_t* transport, uint8_t byte) {
    uint8_t echo;

    if (write(transport->handle, &byte, 1) != 1)
        return transport_status_communication_error;

    // handle synchronous echo byte
    return transport_recv_byte(transport, &echo);
}

transport_status_t transport_recv_bytes(const transport_t* transport, uint8_t* buf, size_t len) {
    transport_status_t r = transport_status_ok;
    size_t recv;

    for (recv = 0; recv < len; ++recv) {
        r = transport_recv_byte(transport, &buf[recv]);
        if (r != transport_status_ok) {
            LOG_ERROR("transport_recv_byte failed: %d", r);
            break;
        }
    }

    return r;
}

transport_status_t transport_send_bytes(const transport_t* transport, const uint8_t* bytes, size_t len) {
    transport_status_t r = transport_status_ok;
    size_t sent;

    for (sent = 0; sent != len; ++sent) {
        r = transport_send_byte(transport, bytes[sent]);
        if (r != transport_status_ok) {
            LOG_ERROR("transport_send_byte failed: %d", r);
            break;
        }
    }

    return r;
}
