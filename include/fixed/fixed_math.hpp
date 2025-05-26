#ifndef FIXED_MATH_HPP
#define FIXED_MATH_HPP

#include "fixed.hpp"

#define TRIG_RESOLUTION 256                     // 0-255 index = 0-360 degrees
#define ATAN_TABLE_SIZE 33                      // covers 0 to 2.0 in 1/16 steps (12.4 max ≈ 2.0)
#define LUT_RESOLUTION 256                      // 0 to 16 in steps of 1/16
#define LUT_STEP_FIXED (1 << (FIXED_SHIFT - 4)) // 1/16 in 12.4 = 0x0010

class FixedMath {
public:
    static void init();

    static Fixed sin(uint8_t angle);
    static Fixed cos(uint8_t angle);
    static uint8_t atan2(Fixed y, Fixed x);

    static Fixed sqrt(Fixed x);
    static Fixed log2(Fixed x);
    static Fixed log(Fixed x);
    static Fixed pow(Fixed base, Fixed exp);

private:
    static Fixed sinTable[TRIG_RESOLUTION];
    static uint8_t atan2Table[ATAN_TABLE_SIZE];

    static Fixed sqrtLUT[LUT_RESOLUTION];
    static Fixed logLUT[LUT_RESOLUTION];
};

#endif
