// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "com.h"

#include <sys/ioctl.h>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

io_status_t com_init(HANDLE *hcom, const char *com_name, uint32_t baudrate) {
    struct termios options = { 0 };
    HANDLE h;
    int ret;

    h = open(com_name, O_RDWR | O_NOCTTY);

    if (h == -1) {
        return IO_ERROR__NO_SUCH_DEVICE;
    }

    ret = tcflush(h, TCIOFLUSH);

    if (ret) {
        close(h);

        return IO_ERROR__UNSUCCESSFUL;
    }

    //tcgetattr will success if tcflush call is successfull
    tcgetattr(h, &options);

    //set i/o speed to BAUDRATE
    ret = cfsetispeed(&options, baudrate);

    //if baudrate is invalid, cfsetospeed will also fail
    if (ret) {
        close(h);

        return IO_ERROR__INVALID_ARG;
    }

    cfsetospeed(&options, baudrate);

    //configure to raw mode (no echo, send raw symbols). Can not fail
    cfmakeraw(&options);

    //set only 1 stop bit
    options.c_cflag &= ~CSTOPB;

    //set 1ms timeout for each symbol
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 1;

    //apply new port settings
    ret = tcsetattr(h, TCSANOW, &options);

    if (ret) {
        close(h);

        return IO_ERROR__INVALID_ARG;
    }

    //handshake off
    int flag = TIOCM_RTS;
    ret = ioctl(h, TIOCMBIC, &flag);

    if (ret) {
        close(h);

        return IO_ERROR__UNSUCCESSFUL;
    }

    flag = TIOCM_DTR;
    ret = ioctl(h, TIOCMBIC, &flag);

    if (ret) {
        close(h);

        return IO_ERROR__UNSUCCESSFUL;
    }

    ret = tcflush(h, TCIOFLUSH);

    if (ret) {
        close(h);

        return IO_ERROR__UNSUCCESSFUL;
    }

    *hcom = h;

    return IO_ERROR__OK;
}

io_status_t com_deinit(HANDLE hcom) {
    if (close(hcom) == -1)
        return IO_ERROR__CLOSE;

    return IO_ERROR__OK;
}
