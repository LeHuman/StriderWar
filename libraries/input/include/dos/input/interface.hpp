#pragma once
#include <stdint.h>

namespace DOS {
namespace Input {
    struct Interface {
        int16_t x;
        int16_t y;
        bool fire;
        bool alt;

        Interface() : x(0), y(0), fire(false), alt(false) {};
    };
} // namespace Input
} // namespace DOS
