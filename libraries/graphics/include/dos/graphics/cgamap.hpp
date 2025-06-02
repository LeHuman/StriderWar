#pragma once

#include <stdint.h>

namespace DOS {
namespace CGA {

    enum Transparency {
        NONE,
        SEMI,
        PERFECT,
    };

    int display_cga(const char *filename, Transparency transparent = SEMI);
}; // namespace CGA
}; // namespace DOS
