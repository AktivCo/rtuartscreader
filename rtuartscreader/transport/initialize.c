// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/transport/initialize.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <rtuartscreader/hardware/hardware.h>
#include <rtuartscreader/transport/detail/error.h>
#include <rtuartscreader/transport/detail/transmit_params.h>


static transport_status_t transport_setup_serial_settings(const transport_t* transport) {
    struct termios options = { 0 };

    int ret = tcgetattr(transport->handle, &options);
    LOG_RETURN_ON_OS_ERROR(ret);

    ret = cfsetispeed(&options, transport->params.transmit_speed.baudrate);
    LOG_RETURN_ON_OS_ERROR(ret);

    ret = cfsetospeed(&options, transport->params.transmit_speed.baudrate);
    LOG_RETURN_ON_OS_ERROR(ret);

    cfmakeraw(&options);

    options.c_cflag |= CSTOPB | PARENB;
    options.c_cflag &= ~PARODD;

    options.c_cc[VMIN] = 0;

    options.c_cc[VTIME] = transport->params.wt_ds;

    ret = tcsetattr(transport->handle, TCSANOW, &options);
    LOG_RETURN_ON_OS_ERROR(ret);

    ret = tcflush(transport->handle, TCIOFLUSH);
    LOG_RETURN_ON_OS_ERROR(ret);

    return transport_status_ok;
}

transport_status_t transport_initialize_impl(transport_t* transport, const char* reader_name) {
    hw_status_t hw_r;
    transport_status_t r;
    int os_r;

    transport->params = *transmit_params_default();

    transport->handle = open(reader_name, O_RDWR | O_NOCTTY);
    if (transport->handle == -1) {
        LOG_OS_ERROR(transport->handle);
        r = transport_status_os_error;
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

    hw_r = hw_start_clock(transport->params.transmit_speed.freq);
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
    LOG_RETURN_ON_OS_ERROR(os_r);
err_label:
    return r;
}

transport_status_t transport_reinitialize_impl(transport_t* transport, const transmit_params_t* params) {
    transport->params = *params;

    transport_status_t r = transport_setup_serial_settings(transport);
    POPULATE_ERROR(r, transport_status_ok, r);

    hw_status_t hw_r = hw_stop_clock();
    RETURN_ON_HW_ERROR(hw_r);

    hw_r = hw_start_clock(transport->params.transmit_speed.freq);
    RETURN_ON_HW_ERROR(hw_r);

    return transport_status_ok;
}

transport_status_t transport_deinitialize_impl(const transport_t* transport) {
    hw_status_t r = hw_stop_clock();
    RETURN_ON_HW_ERROR(r);

    r = hw_rst_deinitialize();
    RETURN_ON_HW_ERROR(r);

    hw_deinitialize();

    int os_r = close(transport->handle);
    LOG_RETURN_ON_OS_ERROR(os_r);

    return transport_status_ok;
}

#define PIMPL_NAME_PREFIX transport_initialize
#define PIMPL_FUNCTIONS_DECLARATION_PATH <rtuartscreader/transport/detail/transport_initialize_functions.h>
#include <rtuartscreader/pimpl/source.h>