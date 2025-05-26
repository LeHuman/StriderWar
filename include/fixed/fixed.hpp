#ifndef FIXED_HPP
#define FIXED_HPP

#include <stdint.h>

#define FIXED_SHIFT 4
#define FIXED_SCALE (1 << FIXED_SHIFT)

class Fixed {
    int16_t value;

public:
    Fixed() : value(0) {}
    Fixed(int integer) : value(integer << FIXED_SHIFT) {}
    Fixed(float f) : value((int16_t)(f * FIXED_SCALE)) {}
    Fixed(int16_t raw, bool rawTag) : value(raw) {} // internal use only

    static Fixed fromRaw(int16_t raw) { return Fixed(raw, true); }
    int16_t raw() const { return value; }

    int toInt() const { return value >> FIXED_SHIFT; }
    float toFloat() const { return value / (float)FIXED_SCALE; }

    // Operators
    Fixed operator+(Fixed rhs) const { return fromRaw(value + rhs.value); }
    Fixed operator-(Fixed rhs) const { return fromRaw(value - rhs.value); }
    Fixed operator*(Fixed rhs) const { return fromRaw((int16_t)((value * rhs.value) >> FIXED_SHIFT)); }
    Fixed operator/(Fixed rhs) const { return fromRaw((int16_t)(((int32_t)value << FIXED_SHIFT) / rhs.value)); }

    Fixed &operator+=(Fixed rhs) {
        value += rhs.value;
        return *this;
    }
    Fixed &operator-=(Fixed rhs) {
        value -= rhs.value;
        return *this;
    }

    bool operator==(Fixed rhs) const { return value == rhs.value; }
    bool operator<(Fixed rhs) const { return value < rhs.value; }
    bool operator>(Fixed rhs) const { return value > rhs.value; }
};

#endif
