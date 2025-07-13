#pragma once

#include <dos/graphics.hpp>
#include <dos/input.hpp>
#include <dos/sound.hpp>
#include <dos/time.hpp>

namespace debug {

static inline void draw_joystick_A() {
    const uint16_t x = DOS::Input::Joystick::playerA.x + (DOS::Video::WIDTH / 2);
    const uint16_t y = DOS::Input::Joystick::playerA.y + (DOS::Video::HEIGHT / 2);
    static DOS::Draw::Line last = {0, 0, 1, 1};
    const DOS::Draw::Line line = {DOS::Video::WIDTH / 2, DOS::Video::HEIGHT / 2, x, y};
    DOS::Draw::line(last, 0);
    DOS::Draw::line(line, 1);
    last = line;
}

} // namespace debug
