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
    math::Fixed x;
    math::Fixed y;
    math::Fixed vx;
    math::Fixed vy;

    uint8_t color;
    bool enabled;
    bool has_physics;

    struct bounce_t {
        bool x;
        bool y;
        math::Fixed angle;

        bounce_t() : x(false), y(false) {}
    } bounce;

    Strider() : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(0), y(0), vx(0), vy(0), color(1), enabled(false), has_physics(false) {}
    Strider(int x, int y) : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(x), y(y), vx(0), vy(0), color(1), enabled(false), has_physics(false) {}
    Strider(int x, int y, int vx, int vy) : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(x), y(y), vx(vx), vy(vy), color(1), enabled(false), has_physics(false) {}

    void set_priority(uint8_t priority);

    void pulse(math::Fixed vx, math::Fixed vy) {
        this->vx += vx;
        this->vy += vy;
    }

    void step();

    void draw(bool force = false);
};
