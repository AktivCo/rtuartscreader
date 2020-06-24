#pragma once

#include <iomanip>
#include <iostream>

#include <rtuartscreader/transport/transport.h>

template<typename T>
void printBuffer(std::ostream& ostr, const T& buffer, size_t width = 16) {
    size_t i = 0;
    for (auto it = std::begin(buffer); it != std::end(buffer); ++it, ++i) {
        if (i == width) {
            ostr << std::endl;
            i = 0;
        }

        ostr << std::setw(2) << std::setfill('0') << std::hex << static_cast<uint32_t>(*it) << ' ' << std::dec;
    }
}

std::ostream& operator<<(std::ostream& ostr, const transmit_params_t& transport_params);