#include "fixed_math.hpp"

#include <math.h> // only for init

Fixed FixedMath::sinTable[TRIG_RESOLUTION];
uint8_t FixedMath::atan2Table[ATAN_TABLE_SIZE];
Fixed FixedMath::sqrtLUT[LUT_RESOLUTION];
Fixed FixedMath::logLUT[LUT_RESOLUTION];

void FixedMath::init() {
    for (int i = 0; i < TRIG_RESOLUTION; ++i) {
        float angle = (2.0f * 3.14159265f * i) / TRIG_RESOLUTION;
        sinTable[i] = Fixed(sin(angle));
    }

    for (int i = 0; i < ATAN_TABLE_SIZE; ++i) {
        float ratio = i / 16.0f;
        float angle = atan(ratio); // radians
        int index = (int)(angle * (TRIG_RESOLUTION / (2.0f * 3.1415926f)) + 0.5f);
        atan2Table[i] = (uint8_t)index;
    }

    // sqrt LUT: sqrt(i/16.0)
    for (int i = 0; i < LUT_RESOLUTION; ++i) {
        float f = i / 16.0f;
        sqrtLUT[i] = Fixed(sqrt(f));
    }

    // log LUT: log(i/16.0)
    for (int i = 1; i < LUT_RESOLUTION; ++i) {
        float f = i / 16.0f;
        logLUT[i] = Fixed(log(f));
    }
    logLUT[0] = Fixed(0); // log(0) undefined, set to 0 or min
}

Fixed FixedMath::sin(uint8_t angle) {
    return sinTable[angle % TRIG_RESOLUTION];
}

Fixed FixedMath::cos(uint8_t angle) {
    return sinTable[(angle + TRIG_RESOLUTION / 4) % TRIG_RESOLUTION];
}

static inline Fixed fixed_abs(Fixed x) {
    return Fixed::fromRaw(x.raw() < 0 ? -x.raw() : x.raw());
}

uint8_t FixedMath::atan2(Fixed y, Fixed x) {
    if (x.raw() == 0 && y.raw() == 0)
        return 0;

    Fixed absY = fixed_abs(y);
    Fixed absX = fixed_abs(x);

    Fixed ratio;
    bool yIsBigger = absY.raw() > absX.raw();

    if (yIsBigger) {
        ratio = Fixed::fromRaw((absX.raw() << FIXED_SHIFT) / absY.raw());
    } else {
        ratio = Fixed::fromRaw((absY.raw() << FIXED_SHIFT) / absX.raw());
    }

    int index = ratio.raw() >> 1; // convert 12.4 to 8.3: shift by 1
    if (index >= ATAN_TABLE_SIZE)
        index = ATAN_TABLE_SIZE - 1;

    uint8_t baseAngle = atan2Table[index];

    if (x.raw() >= 0 && y.raw() >= 0) {
        return yIsBigger ? 64 - baseAngle : baseAngle;
    } else if (x.raw() < 0 && y.raw() >= 0) {
        return yIsBigger ? 64 + baseAngle : 128 - baseAngle;
    } else if (x.raw() < 0 && y.raw() < 0) {
        return yIsBigger ? 192 - baseAngle : 128 + baseAngle;
    } else {
        return yIsBigger ? 192 + baseAngle : 256 - baseAngle;
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
