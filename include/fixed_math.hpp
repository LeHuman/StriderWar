#pragma once

#include "fixed.hpp"

#define TRIG_RESOLUTION 512                     // 0-255 index = 0-360 degrees
#define ATAN2_LUT_BITS 9
#define ATAN2_LUT_SIZE 512
#define LUT_RESOLUTION 512                      // 0 to 16 in steps of 1/16
#define LUT_STEP_FIXED (1 << (FIXED_SHIFT - 4)) // 1/16 in 12.4 = 0x0010

class FixedMath {
public:
    static const Fixed PI;

    static Fixed sin(uint16_t angle);
    static Fixed cos(uint16_t angle);
    static uint16_t atan2(Fixed y, Fixed x);

    static Fixed sqrt(Fixed x);
    static Fixed log2(Fixed x);
    static Fixed log(Fixed x);
    static Fixed pow(Fixed base, Fixed exp);
};
