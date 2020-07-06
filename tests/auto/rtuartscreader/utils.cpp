// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "utils.h"

#include <sstream>
#include <string>

using namespace std;

#define CASE_BAUDRATE(BAUDRATE) \
    case BAUDRATE: return #BAUDRATE

string toString(speed_t baudrate) {
    switch (baudrate) {
        CASE_BAUDRATE(B75);
        CASE_BAUDRATE(B110);
        CASE_BAUDRATE(B134);
        CASE_BAUDRATE(B150);
        CASE_BAUDRATE(B300);
        CASE_BAUDRATE(B600);
        CASE_BAUDRATE(B1200);
        CASE_BAUDRATE(B1800);
        CASE_BAUDRATE(B2400);
        CASE_BAUDRATE(B4800);
        CASE_BAUDRATE(B9600);
        CASE_BAUDRATE(B19200);
        CASE_BAUDRATE(B38400);
        CASE_BAUDRATE(B57600);
        CASE_BAUDRATE(B115200);
        CASE_BAUDRATE(B230400);
    default: {
        stringstream ss;
        ss << "unknown baudrate constant " << static_cast<uint32_t>(baudrate);
        return ss.str();
    }
    }
}

#undef CASE_BAUDRATE

ostream& operator<<(ostream& ostr, const transmit_params_t& transport_params) {
    ostr << "transmit_speed.freq: " << transport_params.transmit_speed.freq << endl;
    ostr << "transmit_speed.baudrate: " << toString(transport_params.transmit_speed.baudrate) << endl;
    ostr << "etu (periods): " << transport_params.etu << endl;
    ostr << "extra_gt_us: " << static_cast<uint32_t>(transport_params.extra_gt_us) << endl;
    ostr << "wt_ds: " << static_cast<uint32_t>(transport_params.wt_ds) << endl;
    return ostr;
}