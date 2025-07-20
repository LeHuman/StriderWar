#pragma once

#include <stdint.h>

namespace DOS {
namespace Draw {
    static const uint16_t COLOR_MAX = 4;

    struct Line {
        uint16_t x0;
        uint16_t y0;
        uint16_t x1;
        uint16_t y1;
    };

    void clear();

#ifdef ENABLE_GRAPHICS_TEXT
    void text(int x, int y, char *string, uint8_t color);
#else
#define text(x, y, s, c) COLOR_MAX
#endif

    void line(int x0, int y0, int x1, int y1, uint8_t color);
    void rectangle(int x0, int y0, int x1, int y1, uint8_t color, bool fill = true);

    inline void line(const Line &line, uint8_t color) {
        DOS::Draw::line(line.x0, line.y0, line.x1, line.y1, color);
    }

    inline void pixel(int x, int y, uint8_t color) {
        line(x, y, x, y, color);
    }

}; // namespace Draw
}; // namespace DOS
