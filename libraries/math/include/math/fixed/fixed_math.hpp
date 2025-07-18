#pragma once

#include "fixed.hpp"

#define TRIG_RESOLUTION 127                     // 0-255 index = 0-360 degrees
#define ATAN2_LUT_BITS 7
#define ATAN2_LUT_SIZE 127
#define LUT_RESOLUTION 127                      // 0 to 16 in steps of 1/16
#define LUT_STEP_FIXED (1 << (FIXED_SHIFT - 4)) // 1/16 in 12.4 = 0x0010

namespace math {
namespace fixed {
    static const Fixed PI = 3.14159265f;

    Fixed sin(uint16_t angle);
    Fixed cos(uint16_t angle);
    uint16_t atan2(Fixed y, Fixed x);
    static inline Fixed abs(Fixed x) {
        return Fixed::fromRaw(x.raw() < 0 ? -x.raw() : x.raw());
    }

    Fixed sqrt(Fixed x);
    Fixed log2(Fixed x);
    Fixed log(Fixed x);
    Fixed pow(Fixed base, Fixed exp);
}; // namespace fixed
}; // namespace math
