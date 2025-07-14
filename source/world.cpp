#include "world.hpp"

#include <utility>

#include <dos/graphics.hpp>
#include <dos/math.hpp>
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

void explode(Strider &bullet) {
    static const int blast_radius = 10;
    const Fixed &x = bullet.x;
    const Fixed &y = bullet.y;

    DOS::Draw::pixel(x - blast_radius, y - blast_radius, 2);
    DOS::Draw::pixel(x - blast_radius, y + blast_radius, 2);
    DOS::Draw::pixel(x + blast_radius, y - blast_radius, 2);
    DOS::Draw::pixel(x + blast_radius, y + blast_radius, 2);

    for (size_t i = 0; i < current_players; i++) {
        const Fixed &sx = players[i]->ship.x;
        const Fixed &sy = players[i]->ship.y;

        const int dx = (int)(sx - x) / 2;
        const int dy = (int)(sy - y) / 2;

        if (((dx * dx) + (dy * dy)) < (blast_radius * blast_radius / 2)) {
            DOS::Draw::line(x, y, sx, sy, 2);
            players[i]->ship.pulse(rand() - (RAND_MAX / 2), rand() - (RAND_MAX / 2));
            debug::serial_print("hit\n");
        }
    }
}

} // namespace world
