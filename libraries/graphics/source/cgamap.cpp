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

    uint8_t sprite_memory[SPRITE_MEMORY_SIZE] = {0};

    SpriteBank sprite_bank = {(uint16_t *)&sprite_memory[0], (uint16_t *)&sprite_memory[2]};
    Sprite sprite;

    int display_sprite(const Sprite &sprite, uint8_t color, Transparency transparent, int8_t x_offset) {
        // Turn color into a mask for all 4 pixels per byte
        static const uint8_t color_map[4] = {0x00, 0x55, 0xAA, 0xFF};
        color = color_map[color & 0x03];

        if ((color == 0) && (transparent != NONE)) {
            return 0;
        }

        for (uint8_t i = 0; i < sprite.length; i++) {
            uint8_t far *vram = (uint8_t far *)MK_FP(DOS::Video::VIDEO_MEMORY, (sprite.offsets[i] + x_offset));
            const uint8_t &mask = sprite.masks[i];
            const uint8_t byte = color & mask;

            // TODO: Is this slower?
            if (*vram == byte) {
                continue;
            }

            switch (transparent) {
                case PERFECT: {
                    if (*vram) {
                        *vram &= ~mask;
                        *vram |= byte;
                    } else {
                        *vram = byte;
                    }
                    break;
                }
                case SEMI:
                    if (byte) {
                        *vram = byte;
                    }
                    break;
                default:
                    *vram = byte;
                    break;
            }
        }

        return 0;
    }

    Error load_sprite(uint16_t id, uint8_t color, Transparency transparent, int8_t x_offset) {
        if (*sprite_bank.length <= id) {
            return Value_Sprite_Load;
        }

        const uint16_t base_i = sprite_bank.sprite_index[id];

        sprite.length = *((uint16_t *)&sprite_memory[base_i]);
        sprite.offsets = (uint16_t *)&sprite_memory[base_i + sizeof(uint16_t)];
        sprite.masks = &sprite_memory[base_i + (sprite.length * sizeof(uint16_t)) + sizeof(uint16_t)];

        display_sprite(sprite, color, transparent, x_offset);

        return None;
    }

    Error load_sprites(const char *filename) {
        FILE *f = fopen(filename, "rb");
        Error code = None;

        if (!f) {
            code = File_Open_Sprite;
        }

        uint16_t file_size = 0;
        if (!code && (fread((void *)&file_size, 1, sizeof(uint16_t), f) != sizeof(uint16_t))) {
            code = File_Read_Sprite_Size;
        }
        file_size -= sizeof(uint16_t);

        if (!code && (fread((void *)sprite_memory, 1, file_size, f) != file_size)) {
            code = File_Read_Sprite_Data;
        }

        fclose(f);

        for (size_t i = 0; i < *sprite_bank.length; i++) {
            load_sprite(i, (i % 3) + 1, NONE, 0);
        }

        return code;
    }

    int display_cga(const char *filename, Transparency transparent) {
        FILE *f = fopen(filename, "rb");
        if (!f) {
            fclose(f);
            return -1;
        }

        for (int y = 0; y < DOS::Video::HEIGHT; ++y) {
            uint8_t line[DOS::Video::BYTES_PER_LINE];
            int pos = 0;

            if (fread(line, 1, 80, f) != 80) {
                fclose(f);
                return -1;
            }

            const unsigned offset = (y / 2) * DOS::Video::BYTES_PER_LINE + (y % 2) * 0x2000;
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

        fclose(f);
        return 0;
    }
} // namespace CGA
} // namespace DOS
