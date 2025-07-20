#pragma once

#include "fixed.hpp"

namespace math {
namespace fixed {
    static inline Fixed abs(Fixed x) {
        return Fixed::fromRaw(x.raw() < 0 ? -x.raw() : x.raw());
    }
}; // namespace fixed
}; // namespace math
