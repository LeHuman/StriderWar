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

inline static void joystick_draw_A() {}

inline static void loop_time_start() {}
inline static void loop_time_stop() {}

inline static void sprite_trigger_rand() {}

inline static void serial_init() {}
inline static void serial_print(const char *str) {}
inline static void serial_printf(const char *fmt, ...) {}

#endif

} // namespace debug
