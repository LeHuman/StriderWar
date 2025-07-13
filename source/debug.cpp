#include "debug.hpp"

#include <stdlib.h>

namespace debug {

void joystick_draw_A() {
    const uint16_t x = DOS::Input::Joystick::playerA.x + (DOS::Video::WIDTH / 2);
    const uint16_t y = DOS::Input::Joystick::playerA.y + (DOS::Video::HEIGHT / 2);
    static DOS::Draw::Line last = {0, 0, 1, 1};
    const DOS::Draw::Line line = {DOS::Video::WIDTH / 2, DOS::Video::HEIGHT / 2, x, y};
    DOS::Draw::line(last, 0);
    DOS::Draw::line(line, 1);
    last = line;
}

inline size_t increment_color(size_t &color) {
    return ((color + 1) % (DOS::Draw::COLOR_MAX - 1)) + 1;
}

DOS::Time::Timer timer;

void loop_time_start() {
    timer.beginFrame();
}

void loop_time_stop() {
    static uint16_t ty = 0;
    static size_t color = 0;

    uint16_t time = timer.elapsedMs();
    uint8_t dist = time / 50;
    color = increment_color(color);

    DOS::Draw::line(dist, ty, dist * 2, ty, 0);
    DOS::Draw::line(0, ty, dist, ty, color);

    ty = (ty + 1) % DOS::Video::HEIGHT;
}

void sprite_trigger_rand() {

    static size_t sprite_i = 0;
    static size_t sprite_c = 0;

    DOS::CGA::load_sprite(sprite_i, sprite_c, DOS::CGA::PERFECT, 0);
    sprite_i = abs(rand()) % *DOS::CGA::sprite_bank.length;
    sprite_c = increment_color(sprite_c);
}

} // namespace debug
