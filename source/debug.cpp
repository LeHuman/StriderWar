#include "debug.hpp"

#include <conio.h>
#include <dos.h>
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

#define COM1 0x3F8
#define LSR 5 // Line Status Register

void serial_out(char c) {
    while ((inp(COM1 + LSR) & 0x20) == 0)
        ;
    outp(COM1, c);
}

void serial_print(const char *str) {
    while (*str) {
        serial_out(*str++);
    }
}

void serial_printf(const char *fmt, ...) {
    char buf[256];            // You can adjust size for your RAM
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args); // Format to buffer
    va_end(args);

    serial_print(buf);        // Send out byte by byte
}

void serial_init() {
    // 9600 baud on 1.8432 MHz clock: divisor = 12
    // 115200 baud : divisor = 1
    outp(COM1 + 3, 0x80); // Set DLAB = 1
    outp(COM1 + 0, 1);   // Divisor LSB
    outp(COM1 + 1, 0);    // Divisor MSB
    outp(COM1 + 3, 0x03); // 8 bits, no parity, 1 stop (DLAB=0)
    outp(COM1 + 2, 0xC7); // Enable FIFO, clear them, trigger level 14 bytes
    outp(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

} // namespace debug
