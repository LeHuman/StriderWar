#include "fixed_math.hpp"

#include <math.h> // only for init

#include "generated_luts.hpp"

static const float M_PI = 3.14159265f;

Fixed FixedMath::sin(uint16_t angle) {
    return sinTable[angle % TRIG_RESOLUTION];
}

Fixed FixedMath::cos(uint16_t angle) {
    return sinTable[(angle + TRIG_RESOLUTION / 4) % TRIG_RESOLUTION];
}

static inline Fixed fixed_abs(Fixed x) {
    return Fixed::fromRaw(x.raw() < 0 ? -x.raw() : x.raw());
}
uint16_t FixedMath::atan2(Fixed y, Fixed x) {
    if (x.raw() == 0 && y.raw() == 0)
        return 0;

    Fixed absY = fixed_abs(y);
    Fixed absX = fixed_abs(x);

    bool yIsBigger = absY.raw() > absX.raw();
    Fixed ratio = yIsBigger
                      ? Fixed::fromRaw((absX.raw() << FIXED_SHIFT) / absY.raw())
                      : Fixed::fromRaw((absY.raw() << FIXED_SHIFT) / absX.raw());

    int index = ratio.raw() << (ATAN2_LUT_BITS - FIXED_SHIFT);
    if (index >= ATAN2_LUT_SIZE) {
        index = ATAN2_LUT_SIZE - 1;
    }

    uint16_t baseAngle = atan2Table[index]; // range 0-65535 for 0-90°

    static const uint16_t QUADRANT_ANGLE = ATAN2_LUT_SIZE / 4;

    // Reflect based on quadrant
    if (x.raw() >= 0 && y.raw() >= 0) {
        return yIsBigger ? (QUADRANT_ANGLE - baseAngle) : baseAngle;
    } else if (x.raw() < 0 && y.raw() >= 0) {
        return yIsBigger ? (QUADRANT_ANGLE + baseAngle) : (2 * QUADRANT_ANGLE - baseAngle);
    } else if (x.raw() < 0 && y.raw() < 0) {
        return yIsBigger ? (3 * QUADRANT_ANGLE - baseAngle) : (2 * QUADRANT_ANGLE + baseAngle);
    } else {
        return yIsBigger ? (3 * QUADRANT_ANGLE + baseAngle) : (4 * QUADRANT_ANGLE - baseAngle);
    }
}

Fixed FixedMath::sqrt(Fixed x) {
    int index = x.raw() >> (FIXED_SHIFT - 4); // 12.4 → 8-bit index
    if (index >= LUT_RESOLUTION)
        index = LUT_RESOLUTION - 1;
    return sqrtLUT[index];
}

Fixed FixedMath::log(Fixed x) {
    int index = x.raw() >> (FIXED_SHIFT - 4);
    if (index <= 0)
        return logLUT[1]; // avoid log(0)
    if (index >= LUT_RESOLUTION)
        index = LUT_RESOLUTION - 1;
    return logLUT[index];
}

Fixed FixedMath::log2(Fixed x) {
    // log2(x) = log(x) / log(2)
    static const Fixed LOG2 = Fixed(0.6931f);
    return FixedMath::log(x) / LOG2;
}

Fixed FixedMath::pow(Fixed base, Fixed exp) {
    // Approximate: base^exp ≈ exp * log(base)
    Fixed l = FixedMath::log(base);
    Fixed result = exp * l;

    // exp(log(base) * exp) ≈ 2^x (skip for now — fallback)
    return result;
}
