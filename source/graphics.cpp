#include "graphics.hpp"

#include <dos.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

void Graphics::init() {
    _setvideomode(_MRES4COLOR);
    _selectpalette(2);
}

void Graphics::pixel(int x, int y, uint8_t color) {
    _setcolor(color);
    _setpixel(x, y);
}

void Graphics::line(int x0, int y0, int x1, int y1, uint8_t color) {
    _setcolor(color);
    _moveto(x0, y0);
    _lineto(x1, y1);
}

void Graphics::text(int x, int y, char *string, uint8_t color) {
    _setcolor(color);
    _grtext(x, y, string);
}

void Graphics::clear() {
    _clearscreen(0);
}
