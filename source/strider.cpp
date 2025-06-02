#include "strider.hpp"

Strider::Strider() : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(0), y(0), vx(0), vy(0), color(1), enabled(false), has_physics(false) {}
Strider::Strider(int x, int y) : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(x), y(y), vx(0), vy(0), color(1), enabled(false), has_physics(false) {}
Strider::Strider(int x, int y, int vx, int vy) : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(x), y(y), vx(vx), vy(vy), color(1), enabled(false), has_physics(false) {}

void Strider::set_priority(uint8_t priority) {
    static const uint8_t priority_map[6] = {
        7,
        15,
        23,
        31,
        39,
        47,
    };

    static const int trail_max = (sizeof(trail) / sizeof(trail[0]));

    trail_count = trail_max / (sizeof(priority_map) / sizeof(priority_map[0])) / (priority + 1);

    if (trail_count <= 0) {
        trail_count = 2;
    }

    if (trail_count >= trail_max) {
        trail_count = trail_max;
    }

    draw_counter_max = priority_map[priority];
}

void Strider::pulse(Fixed vx, Fixed vy) {
    this->vx += vx;
    this->vy += vy;
}

void Strider::step() {
    if (!enabled) {
        return;
    }

    if (has_physics) {
        // vy += gravity;

        vx *= friction;
        vy *= friction;
    }

    x += vx;
    y += vy;

    if (x <= DOS::Video::X_MIN) {
        x = DOS::Video::X_MIN;
        x += 0.01f;
        bounce.x = true;
    }

    if (x >= DOS::Video::X_MAX) {
        x = DOS::Video::X_MAX;
        x -= 0.01f;
        bounce.x = true;
    }

    if (y <= DOS::Video::Y_MIN) {
        y = DOS::Video::Y_MIN;
        y += 0.01f;
        bounce.y = true;
    }

    if (y >= DOS::Video::Y_MAX) {
        y = DOS::Video::Y_MAX;
        y -= 0.01f;
        bounce.y = true;
    }

    if (has_physics && (bounce.y || bounce.x)) {
        bounce.angle = DOS::Math::Fix::atan2(vy, vx);
        vx *= bounce_mult;
        vy *= bounce_mult;
    }

    draw_counter_limit = (((draw_counter_max - DOS::Math::Fix::sqrt(vx * vx + vy * vy).toInt() * (draw_counter_max / 4)) & draw_counter_max) / 2) | 1;
}

void Strider::draw(bool force) {
    // TODO: still clear lines after disable
    if (!force && (!enabled || (draw_counter++ % draw_counter_limit != 0))) {
        return;
    }
    trail[count].x0 = x;
    trail[count].y0 = y;
    trail[count].x1 = x - vx * (tail_mult + (draw_counter_max / 16));
    trail[count].y1 = y - vy * (tail_mult + (draw_counter_max / 16));

    DOS::Draw::line(trail[count], color);
    DOS::Draw::line(trail[last_count], 0);

    last_count = count;
    count++;
    count %= sizeof(trail) / sizeof(trail[0]);
}
