#ifndef FIXED_HPP
#define FIXED_HPP

#include <stdint.h>

#define FIXED_SHIFT 4
#define FIXED_SCALE (1 << FIXED_SHIFT)

#define FLOAT2FIXED(f) ((int16_t)(f * FIXED_SCALE))
#define INT2FIXED(i) ((int16_t)(i << FIXED_SHIFT))

class Fixed {
    int16_t value;

public:
    Fixed() : value(0) {}
    explicit Fixed(int integer) : value((INT2FIXED(integer))) {}
    Fixed(float f) : value(FLOAT2FIXED(f)) {}
    Fixed(int16_t raw, bool rawTag) : value(raw) {} // internal use only

    static Fixed fromFloat(float flt) { return Fixed(flt); }
    static Fixed fromRaw(int16_t raw) { return Fixed(raw, true); }
    int16_t raw() const { return value; }

    int toInt() const { return value >> FIXED_SHIFT; }
    float toFloat() const { return value / (float)FIXED_SCALE; }

    // Operators
    Fixed operator+(Fixed rhs) const { return fromRaw(value + rhs.value); }
    Fixed operator+(float f) const { return fromRaw(value + FLOAT2FIXED(f)); }
    Fixed operator+(int i) const { return fromRaw(value + INT2FIXED(i)); }

    Fixed operator-(Fixed rhs) const { return fromRaw(value - rhs.value); }
    Fixed operator-(float f) const { return fromRaw(value - FLOAT2FIXED(f)); }
    Fixed operator-(int i) const { return fromRaw(value - INT2FIXED(i)); }

    Fixed operator*(Fixed rhs) const { return fromRaw((int16_t)((value * rhs.value) >> FIXED_SHIFT)); }
    Fixed operator*(float f) const { return fromRaw((int16_t)((value * FLOAT2FIXED(f)) >> FIXED_SHIFT)); }
    Fixed operator*(int i) const { return fromRaw((int16_t)((value * INT2FIXED(i)) >> FIXED_SHIFT)); }

    Fixed operator/(Fixed rhs) const { return fromRaw((int16_t)(((int32_t)value << FIXED_SHIFT) / rhs.value)); }
    Fixed operator/(float f) const { return fromRaw((int16_t)(((int32_t)value << FIXED_SHIFT) / FLOAT2FIXED(f))); }
    Fixed operator/(int i) const { return fromRaw((int16_t)(((int32_t)value << FIXED_SHIFT) / INT2FIXED(i))); }

    Fixed &operator+=(Fixed rhs) {
        value += rhs.value;
        return *this;
    }
    Fixed &operator-=(Fixed rhs) {
        value -= rhs.value;
        return *this;
    }
    Fixed &operator*=(Fixed rhs) {
        value = (*this * rhs).value;
        return *this;
    }
    Fixed &operator/=(Fixed rhs) {
        value = (*this / rhs).value;
        return *this;
    }

    bool operator==(Fixed rhs) const { return value == rhs.value; }
    bool operator<(Fixed rhs) const { return value < rhs.value; }
    bool operator>(Fixed rhs) const { return value > rhs.value; }
    bool operator<=(Fixed rhs) const { return value <= rhs.value; }
    bool operator>=(Fixed rhs) const { return value >= rhs.value; }

    operator float() const {
        return value / static_cast<float>(FIXED_SCALE);
    }

    operator uint16_t() const {
        return static_cast<uint16_t>(value >> FIXED_SHIFT);
    }

    operator uint8_t() const {
        return static_cast<uint8_t>(value >> FIXED_SHIFT);
    }

    operator int() const {
        return static_cast<int>(value >> FIXED_SHIFT);
    }
};

#endif
