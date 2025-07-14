#include "dos/graphics/draw.hpp"

#include <dos.h>
#include <graph.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint8_t last_color = 0xFF;
static void set_color(uint8_t color) {
    if (color != last_color) {
        _setcolor(color);
        last_color = color;
    }
}

namespace DOS {
namespace Draw {

    void pixel(int x, int y, uint8_t color) {
        set_color(color);
        _setpixel(x, y);
    }

    void line(int x0, int y0, int x1, int y1, uint8_t color) {
        set_color(color);
        _moveto(x0, y0);
        _lineto(x1, y1);
    }

    void rectangle(int x0, int y0, int x1, int y1, uint8_t color, bool fill) {
        set_color(color);
        _setplotaction(_GPSET);
        _rectangle(fill ? _GFILLINTERIOR : _GBORDER, x0, y0, x1, y1);
    }

    void text(int x, int y, char *string, uint8_t color) {
        set_color(color);
        _grtext(x, y, string);
    }

    void clear() {
        _clearscreen(0);
    }

} // namespace Draw
} // namespace DOS
