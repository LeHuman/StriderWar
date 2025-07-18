#include "world.hpp"

#include <utility>

#include <dos/graphics.hpp>
#include <dos/math.hpp>
#include <rand.hpp>
#include <stdlib.h>

#include "debug.hpp"

namespace world {

size_t current_players = 0;
Player *players[MAX_PLAYERS];

int add_player(Player &player) {
    int id = -1;

    if (current_players < MAX_PLAYERS) {
        id = current_players;
        player.id = current_players;
        players[current_players++] = &player;
    }

    return id;
}

void explode(Bullet &bullet) {
    const int blast_radius = 8 + bullet.mult.damage;
    const Fixed &x = bullet.entity.x;
    const Fixed &y = bullet.entity.y;

    DOS::Draw::pixel(x - bullet.mult.damage, y - bullet.mult.damage, 3);
    DOS::Draw::pixel(x - bullet.mult.damage, y + bullet.mult.damage, 3);
    DOS::Draw::pixel(x + bullet.mult.damage, y - bullet.mult.damage, 3);
    DOS::Draw::pixel(x + bullet.mult.damage, y + bullet.mult.damage, 3);

    for (size_t i = 0; i < current_players; i++) {
        const Fixed &sx = players[i]->ship.entity.x;
        const Fixed &sy = players[i]->ship.entity.y;

        const int dx = (int)(sx - x) / 2;
        const int dy = (int)(sy - y) / 2;

        if (((dx * dx) + (dy * dy)) < (blast_radius * blast_radius / 2)) {
            DOS::Draw::line(x, y, sx, sy, 2);
            players[i]->ship.entity.pulse(random::get(-blast_radius, blast_radius), random::get(-blast_radius, blast_radius));
            players[i]->damage(random::get(1, bullet.mult.damage + 1));
            debug::serial_printf("hit %i\n", bullet.mult.damage);
        }
    }
}

} // namespace world
