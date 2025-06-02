#include "dos/graphics/cgamap.hpp"

#include <conio.h>
#include <dos.h>
#include <graph.h>
#include <i86.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dos/graphics/video.hpp"

namespace DOS {
namespace CGA {
    int display_cga(const char *filename, Transparency transparent) {
        FILE *f = fopen(filename, "rb");
        if (!f) {
            return -1;
        }

        for (int y = 0; y < DOS::Video::HEIGHT; ++y) {
            uint8_t line[DOS::Video::BYTES_PER_LINE];
            int pos = 0;

            if (fread(line, 1, 80, f) != 80) {
                return -1;
            }

            unsigned offset = (y / 2) * DOS::Video::BYTES_PER_LINE + (y % 2) * 0x2000;
            uint8_t far *vram = (uint8_t far *)MK_FP(DOS::Video::VIDEO_MEMORY, offset);
            for (int i = 0; i < DOS::Video::BYTES_PER_LINE; ++i) {
                const uint8_t &byte = line[i];

                switch (transparent) {
                    case PERFECT: {
                        for (int p = 0; p < 4; ++p) {
                            int shift = 6 - (p * 2);
                            uint8_t mask = 0x3 << shift;
                            uint8_t pixel = byte & mask;
                            if (pixel) {
                                vram[i] &= ~mask;
                                vram[i] |= pixel;
                            }
                        }
                        break;
                    }
                    case SEMI:
                        if (byte) {
                            vram[i] = byte;
                        }
                        break;
                    default:
                        vram[i] = byte;
                        break;
                }
            }
        }

        return 0;
    }
} // namespace CGA
} // namespace DOS
