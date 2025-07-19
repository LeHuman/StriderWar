#pragma once

#include <dos/graphics.hpp>
#include <dos/input.hpp>
#include <dos/sound.hpp>
#include <dos/time.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef DOSBOX
#define DEBUG_ENABLE_FUNCTIONS
#endif

namespace debug {

#ifdef DEBUG_ENABLE_FUNCTIONS
void joystick_draw_A();

void loop_time_start();
void loop_time_stop();

void sprite_trigger_rand();

void serial_init();
void serial_print(const char *str);
void serial_printf(const char *fmt, ...);
#else

using namespace DOS::Input::Joystick;

#define joystick_draw_A() LUT_SIZE
#define loop_time_start() LUT_SIZE
#define loop_time_stop() LUT_SIZE
#define sprite_trigger_rand() LUT_SIZE
#define serial_init() LUT_SIZE
#define serial_print(x) LUT_SIZE
#define serial_printf(...) LUT_SIZE

#endif

} // namespace debug
