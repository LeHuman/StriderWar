#include <dos/input.hpp>

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
        static const int wait_time = 25;
        static int cooldown = 0;
        static int wait = 0;

        const Fixed &x = player->ship.entity.x;
        const Fixed &y = player->ship.entity.y;
        const Fixed &vx = player->ship.entity.vx;
        const Fixed &vy = player->ship.entity.vy;

        if ((int)(vx + vy) > 10) {
            debug::serial_printf("%i\n", (int)(vx + vy));
            cooldown += 5;
            return;
        } else if (cooldown > 0) {
            cooldown--;
            return;
        }

        Fixed tx = target->ship.entity.x;
        Fixed ty = target->ship.entity.y;
        const int dx = (int)(tx - x) / 2;
        const int dy = (int)(ty - y) / 2;

        if (wait) {
            fire = false;
            wait++;
            wait %= wait_time;
            alt = wait == 0;
        } else if (((dx * dx) + (dy * dy)) < (target_range * target_range / 2)) {
            fire = true;
            alt = false;
            wait = 1;
        }

        this->x = (int16_t)(int)(tx - x);
        this->y = (int16_t)(int)(ty - y);
    };
};

} // namespace dummy
