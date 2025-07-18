#pragma once

#include <dos/graphics.hpp>
#include <math.hpp>
#include <stdlib.h>

class Strider {
private:
    DOS::Draw::Line trail[4];
    int trail_count;
    int last_count;
    int count;
    uint8_t draw_counter;
    uint8_t draw_counter_limit;
    uint8_t draw_counter_max;

public:
    Fixed x;
    Fixed y;
    Fixed vx;
    Fixed vy;

    uint8_t color;
    bool enabled;
    bool has_physics;

    struct bounce_t {
        bool x;
        bool y;
        Fixed angle;

        bounce_t() : x(false), y(false) {}
    } bounce;

    Strider();
    Strider(int x, int y);
    Strider(int x, int y, int vx, int vy);

    void set_priority(uint8_t priority);

    void pulse(Fixed vx, Fixed vy);

    void step();

    void draw(bool force = false);
};
