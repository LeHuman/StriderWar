#pragma once

#include <stddef.h>
#include <stdint.h>

namespace DOS {
namespace CGA {
    enum Error {
        None,
        File_Open_Sprite,
        File_Read_Sprite_Size,
        File_Read_Sprite_Data,
        Value_Sprite_Load,
    };

    struct Sprite {
        uint16_t length;
        uint16_t *offsets;
        uint8_t *masks;
    };

    struct SpriteBank {
        uint16_t *length;
        uint16_t *sprite_index;
    };

    enum Transparency {
        NONE,
        SEMI,
        PERFECT,
    };

    static const size_t SPRITE_MEMORY_SIZE = 1 << 13;
    extern uint8_t sprite_memory[SPRITE_MEMORY_SIZE];
    extern SpriteBank sprite_bank;

    int display_cga(const char *filename, Transparency transparent = SEMI);
    int display_sprite(const uint16_t *offsets, const uint8_t *masks, uint8_t length, uint8_t color, Transparency transparent, int8_t x_offset);
    Error load_sprite(uint16_t id, uint8_t color, Transparency transparent, int8_t x_offset);
    Error load_sprites(const char *filename);
}; // namespace CGA
}; // namespace DOS
