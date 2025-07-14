#include <dos/input.hpp>

#include "debug.hpp"
#include "player.hpp"
#include "world.hpp"

namespace dummy {

struct Dummy : DOS::Input::Interface {
    Player *player;

    Dummy() : Interface() {};

    void set_player(Player *player) {
        this->player = player;
    }

    void update() {
        x = (int16_t)((int)world::X_CENTER - (int)player->ship.x);
        y = (int16_t)((int)world::Y_CENTER - (int)player->ship.y);
        x *= 10;
        y *= 10;
        
        // alt = true;
        // fire = true;
    };
};

} // namespace dummy
