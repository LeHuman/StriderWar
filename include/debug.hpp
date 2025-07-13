#pragma once

#include <dos/graphics.hpp>
#include <dos/input.hpp>
#include <dos/sound.hpp>
#include <dos/time.hpp>

namespace debug {

void joystick_draw_A();

void loop_time_start();
void loop_time_stop();

void sprite_trigger_rand();

} // namespace debug
