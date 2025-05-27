#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

class Graphics {
public:
    // Fixed to _MRES4COLOR
    static const uint16_t X_MIN = 0;
    static const uint16_t X_MAX = 640 / 2;
    static const uint16_t Y_MIN = 0;
    static const uint16_t Y_MAX = 200;
    static const uint16_t COLOR_MAX = 4;

    struct Line {
        uint16_t x0;
        uint16_t y0;
        uint16_t x1;
        uint16_t y1;
    };

    static void init();
    static void pixel(int x, int y, uint8_t color);
    static void clear();

    static void text(int x, int y, char *string, uint8_t color);
    static void line(int x0, int y0, int x1, int y1, uint8_t color);
    static inline void line(const Line &line, uint8_t color) {
        Graphics::line(line.x0, line.y0, line.x1, line.y1, color);
    }
};

#endif // GRAPHICS_H
