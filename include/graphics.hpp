#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

class Graphics {
public:
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
};

#endif // GRAPHICS_H
