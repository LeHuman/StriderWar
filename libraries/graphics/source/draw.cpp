#include "dos/graphics/draw.hpp"

#include <dos.h>
#include <graph.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace DOS {
namespace Draw {
    void line(int x0, int y0, int x1, int y1, uint8_t color) {
        _setcolor(color);
        _moveto(x0, y0);
        _lineto(x1, y1);
    }

#ifdef ENABLE_GRAPHICS_TEXT
    void text(int x, int y, char *string, uint8_t color) {
        _setcolor(color);
        _grtext(x, y, string);
    }
#endif

    void rectangle(int x0, int y0, int x1, int y1, uint8_t color, bool fill) {
        _setcolor(color);
        _setplotaction(_GPSET);
        _rectangle(fill ? _GFILLINTERIOR : _GBORDER, x0, y0, x1, y1);
    }

    void clear() {
        _clearscreen(0);
    }

} // namespace Draw
} // namespace DOS
