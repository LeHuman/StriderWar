#pragma once

#include <graph.h>
#include <stddef.h>
#include <stdint.h>

namespace DOS {
namespace Video {

    static const size_t VIDEO_MEMORY = 0xB800;

    static const size_t MODE = _MRES4COLOR;
    static const size_t PALETTE = 2;
    static const size_t BYTES_PER_LINE = 80;
    static const size_t BANK_OFFSET = 0x2000;
    static const size_t HEIGHT = 200;
    static const size_t WIDTH = 320;

    // TODO: Move this back to Draw
    static const uint16_t X_MIN = 0;
    static const uint16_t X_MAX = DOS::Video::WIDTH;
    static const uint16_t Y_MIN = 0;
    static const uint16_t Y_MAX = DOS::Video::HEIGHT;

    void initialize();

} // namespace Video
} // namespace DOS
