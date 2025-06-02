#include "player.hpp"

#include <dos/graphics.hpp>
#include <dos/math.hpp>

#include "world.hpp"

Player::Player(Joysticks::Player &input) : enabled_bullets(0), input(input), last_bullet(false), req_bullet(false), bounced(0) {
    ship.x = DOS::Video::X_MAX / 2;
    ship.y = DOS::Video::Y_MAX / 2;
    ship.enabled = true;
    ship.has_physics = true;
    ship.color = 1;
    for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
        bullets[i].color = 2;
        bullets[i].set_priority(1);
    }
}

static const Fixed bullet_vel_cmp = 1;

void Player::step() {
    if (!input.alt) {
        Fixed ix = (Fixed)(input.x - 2) / 6;
        Fixed iy = (Fixed)(input.y - 2) / 6;

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

        if (ship.bounce.angle < DOS::Math::Fix::PI) {
            ship.bounce.angle -= spark_deflect_rad;
        } else {
            ship.bounce.angle += spark_deflect_rad;
        }

        Fixed dx = DOS::Math::Fix::cos(ship.bounce.angle);
        Fixed dy = DOS::Math::Fix::sin(ship.bounce.angle);

        spark.x0 = ship.x - (dx * spark_dist);
        spark.y0 = ship.y - (dy * spark_dist);
        spark.x1 = spark.x0 + (dx * spark_tail);
        spark.y1 = spark.y0 + (dy * spark_tail);
    }

    bool fire = false;

    if (req_bullet != last_bullet && (DOS::Math::Fix::abs(ship.vx) > bullet_vel_cmp || DOS::Math::Fix::abs(ship.vy) > bullet_vel_cmp)) {
        fire = req_bullet;
        last_bullet = req_bullet;
    }

    for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
        Strider &bullet = bullets[i];

        if (fire && !bullet.enabled) {
            fire = false;
            bullet.enabled = true;
            ++enabled_bullets;
            bullet.x = ship.x;
            bullet.y = ship.y;
            bullet.vx = ship.vx * bullet_speed;
            bullet.vy = ship.vy * bullet_speed;
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

void Player::draw() {
    if (bounced) {
        DOS::Draw::line(spark, bounced - 1);
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
