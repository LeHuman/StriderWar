#pragma once

#include <dos/graphics.hpp>
#include <dos/input.hpp>
#include <dos/sound.hpp>
#include <dos/time.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace debug {

void joystick_draw_A();

void loop_time_start();
void loop_time_stop();

void sprite_trigger_rand();

void serial_init();
void serial_print(const char *str);
void serial_printf(const char *fmt, ...);

} // namespace debug
