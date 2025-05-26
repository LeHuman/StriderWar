#include <cmath>
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cube.hpp"
#include "fixed.hpp"
#include "fixed_math.hpp"
#include "graphics.hpp"
#include "i86.h"
#include "joystick.hpp"
#include "sound.hpp"

static const Fixed X_MIN = 0.0f;
static const Fixed X_MAX = 640.0f / 2.0f;
static const Fixed Y_MIN = 0.0f;
static const Fixed Y_MAX = 200.0f;

static const Fixed gravity = 0.45f;
static const Fixed friction = 0.999f;
static const Fixed bounce_mult = -0.92f;
static const Fixed tail_mult = 2.0f;

static const Fixed bullet_speed = 2.0f;
static const Fixed bullet_tail = 2.0f;

class Strider {
private:
    Graphics::Line trail[4];
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

    Strider() : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(0), y(0), vx(0), vy(0), color(1), enabled(false), has_physics(false) {}
    Strider(int x, int y) : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(x), y(y), vx(0), vy(0), color(1), enabled(false), has_physics(false) {}
    Strider(int x, int y, int vx, int vy) : trail_count(2), last_count(0), count(1), draw_counter(0), draw_counter_limit(4), draw_counter_max(7), x(x), y(y), vx(vx), vy(vy), color(1), enabled(false), has_physics(false) {}

    void set_priority(uint8_t priority) {
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

    void pulse(Fixed vx, Fixed vy) {
        this->vx += vx;
        this->vy += vy;
    }

    void step() {
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

        if (x <= X_MIN) {
            x = X_MIN + 0.01f;
            bounce.x = true;
        }

        if (x >= X_MAX) {
            x = X_MAX - 0.01f;
            bounce.x = true;
        }

        if (y <= Y_MIN) {
            y = Y_MIN + 0.01f;
            bounce.y = true;
        }

        if (y >= Y_MAX) {
            y = Y_MAX - 0.01f;
            bounce.y = true;
        }

        if (has_physics && (bounce.y || bounce.x)) {
            bounce.angle = FixedMath::atan2(vy, vx);
            vx *= bounce_mult;
            vy *= bounce_mult;
        }

        draw_counter_limit = (((draw_counter_max - FixedMath::sqrt(vx * vx + vy * vy).toInt() * (draw_counter_max / 4)) & draw_counter_max) / 2) | 1;
    }

    void draw(bool force = false) {
        // TODO: still clear lines after disable
        if (!force && (!enabled || (draw_counter++ % draw_counter_limit != 0))) {
            return;
        }
        trail[count].x0 = x;
        trail[count].y0 = y;
        trail[count].x1 = x - vx * (tail_mult + (draw_counter_max / 16));
        trail[count].y1 = y - vy * (tail_mult + (draw_counter_max / 16));

        Graphics::line(trail[count].x0, trail[count].y0, trail[count].x1, trail[count].y1, color);
        Graphics::line(trail[last_count].x0, trail[last_count].y0, trail[last_count].x1, trail[last_count].y1, 0);

        last_count = count;
        count++;
        count %= sizeof(trail) / sizeof(trail[0]);
    }
};

struct Player {
    Strider ship;
    Strider bullets[2];
    uint8_t enabled_bullets;

    bool last_bullet;
    bool req_bullet;
    int last_bcount;

    int bounced;
    Graphics::Line spark;

    const Joysticks::Player &input;

    Player(Joysticks::Player &input) : enabled_bullets(0), input(input), req_bullet(false), last_bcount(0), bounced(0) {
        ship.x = X_MAX / 2.0f;
        ship.y = Y_MAX / 2.0f;
        ship.enabled = true;
        ship.has_physics = true;
        ship.color = 1;
        for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
            bullets[i].color = 2;
            bullets[i].set_priority(1);
        }
    }

    void step() {
        if (!input.alt) {
            Fixed ix = (Fixed)(input.x - 2) / 8;
            Fixed iy = (Fixed)(input.y - 2) / 8;

            ship.pulse(ix, iy);
        }

        req_bullet = input.fire;

        ship.step();

        if (bounced == 0 && (ship.bounce.x || ship.bounce.y)) {
            bounced = 4;
            ship.bounce.x = false;
            ship.bounce.y = false;

            static const Fixed spark_dist = 0.25f;
            static const Fixed spark_tail = -0.5f;
            static const Fixed spark_deflect_rad = 30.0f;

            if (ship.bounce.angle < FixedMath::PI) {
                ship.bounce.angle -= spark_deflect_rad;
            } else {
                ship.bounce.angle += spark_deflect_rad;
            }

            Fixed dx = FixedMath::cos(ship.bounce.angle);
            Fixed dy = FixedMath::sin(ship.bounce.angle);

            spark.x0 = ship.x - (dx * spark_dist);
            spark.y0 = ship.y - (dy * spark_dist);
            spark.x1 = spark.x0 + (dx * spark_tail);
            spark.y1 = spark.y0 + (dy * spark_tail);
        }

        for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
            Strider &bullet = bullets[i];

            if (last_bcount == 0 && req_bullet && !bullet.enabled) {
                last_bcount = 10;
                ++enabled_bullets;
                bullet.enabled = true;
                bullet.x = ship.x;
                bullet.y = ship.y;
                bullet.vx = ship.vx * bullet_speed;
                bullet.vy = ship.vy * bullet_speed;
            } else if (last_bcount > 0) {
                --last_bcount;
            }

            bullet.step();

            if (bullet.bounce.x || bullet.bounce.y) {
                bullet.draw(true);
                bullet.bounce.x = false;
                bullet.bounce.y = false;
                bullet.enabled = false;
                --enabled_bullets;
            }
        }

        for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
            bullets[i].set_priority(enabled_bullets + 1);
        }
    }

    void draw() {
        if (bounced) {
            Graphics::line(spark.x0, spark.y0, spark.x1, spark.y1, bounced - 1);
            if (bounced == 4) {
                bounced = 1;
            } else {
                bounced = 0;
            }
        }

        ship.draw();
        for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
            bullets[i].draw();
        }
    }
};

int main() {
    Graphics::init();

    Joysticks joysticks(0x201);

    Player playerA(joysticks.playerA);
    Player playerB(joysticks.playerB);

    while (!kbhit()) {
        joysticks.update();

        playerA.step();
        playerB.step();

        playerA.draw();
        playerB.draw();
    }

    getch();
    return 0;
}
