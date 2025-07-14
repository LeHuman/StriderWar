#include "world.hpp"

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

} // namespace world
