#include <dos/input.hpp>
#include <math.hpp>

#include "debug.hpp"
#include "player.hpp"
#include "world.hpp"

namespace dummy {

struct Dummy : DOS::Input::Interface {
    Player *player;
    Player *target;

    Dummy() : Interface() {};

    void set_player(Player *player) {
        this->player = player;
    }

    void set_target(Player *player) {
        this->target = player;
    }

    void update() {
        static const int target_range = 30;
        static const int wait_time = 20;
        static const int back_time_on = 105;
        static const int back_time_mid = 50;
        static const int back_time_off = 90;
        static const int slow_limit = 100;
        static int cooldown = 0;
        static int too_slow = 0;
        static int backtrack = 0;
        static bool backtrack_active = false;
        static int wait = 0;
        static math::Fixed ltx((int)world::X_CENTER);
        static math::Fixed lty((int)world::Y_CENTER);

        const math::Fixed &x = player->ship.entity.x;
        const math::Fixed &y = player->ship.entity.y;
        const math::Fixed &vx = player->ship.entity.vx;
        const math::Fixed &vy = player->ship.entity.vy;
        const math::Fixed s = vx + vy;

        if (s > 10) {
            debug::serial_printf("%i\n", (int)(vx + vy));
            cooldown += 5;
            return;
        } else if (cooldown > 0) {
            cooldown--;
            return;
        }

        math::Fixed tx = target->ship.entity.x;
        math::Fixed ty = target->ship.entity.y;

        if (too_slow >= slow_limit) {
            backtrack_active = true;
            backtrack = back_time_off + 10;
            too_slow = 0;
            ltx = (ty * 2) - (ltx / 2);
            lty = (tx * 2) - (lty / 2);
            ltx += random::get(world::X_MIN, world::X_MAX) / 2;
            lty += random::get(world::Y_MIN, world::Y_MAX) / 2;
        } else if (backtrack_active) {
            if ((--backtrack) <= back_time_off) {
                backtrack = 0;
                backtrack_active = false;
                ltx = (tx + x) / 2;
                lty = (ty + y) / 2;
            } else {
                tx = ltx;
                ty = lty;
            }
        } else {
            backtrack_active = (++backtrack) == back_time_on;
            too_slow += s <= 1;
        }

        if (player->ship.condition.body == Condition::DISABLED) {
            backtrack_active = false;
            wait = wait * (wait_time - (player->bullets[0].mult.damage + player->bullets[1].mult.damage) - s.toInt());
        }

        const int dx = (int)(tx - x) / 2;
        const int dy = (int)(ty - y) / 2;

        if (wait) {
            fire = false;
            wait++;
            wait %= wait_time - (player->bullets[0].mult.damage + player->bullets[1].mult.damage) - s.toInt();
            alt = wait == 0 || world::check_bullet(player->bullets[0], target);
        } else if ((backtrack <= back_time_mid) && (((dx * dx) + (dy * dy)) < (target_range * target_range / 2))) {
            fire = true;
            alt = false;
            wait = 1;
        }

        this->x = (int16_t)(int)(tx - x);
        this->y = (int16_t)(int)(ty - y);

        this->x = (this->x > 50) ? 50 : ((this->x < -50) ? -50 : this->x);
        this->y = (this->y > 50) ? 50 : ((this->y < -50) ? -50 : this->y);
    };
};

} // namespace dummy
