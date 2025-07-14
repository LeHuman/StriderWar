#include "player.hpp"

#include <dos/graphics.hpp>
#include <dos/math.hpp>

#include "world.hpp"

Player::Player(DOS::Input::Interface &input) : id(-1), enabled_bullets(0), input(input), last_bullet(false), req_bullet(false), bounced(0) {
    ship.x = world::X_CENTER;
    ship.y = world::Y_CENTER;
    ship.enabled = true;
    ship.has_physics = true;
    ship.color = 1;
    for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
        bullets[i].color = 2;
        bullets[i].set_priority(1);
    }
}

static const Fixed bullet_vel_cmp = 0.5f;

void Player::step() {
    Fixed ix = (Fixed)(input.x) / 300;
    Fixed iy = (Fixed)(input.y) / 300;

    ship.pulse(ix, iy);

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

    for (size_t i = 0; i < MAX_BULLETS; i++) {
        Strider &bullet = bullets[i];

        // TODO: Delay on reenabled bullet
        if (input.alt && bullet.enabled) {
            bullet.bounce.x = true;
        } else 
        if (fire && !bullet.enabled) {
            fire = false;
            bullet.enabled = true;
            ++enabled_bullets;
            bullet.x = ship.x;
            bullet.y = ship.y;
            bullet.vx = ship.vx * world::bullet_speed;
            bullet.vy = ship.vy * world::bullet_speed;
        }

        bullet.step();

        if (bullet.bounce.x || bullet.bounce.y) {
            bullet.draw(true);
            bullet.bounce.x = false;
            bullet.bounce.y = false;
            bullet.enabled = false;
            --enabled_bullets;
            world::explode(bullet);
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
