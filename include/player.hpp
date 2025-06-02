#pragma once

#include <dos/input.hpp>
#include <stdlib.h>

#include "strider.hpp"

struct Player {
    Strider ship;
    Strider bullets[2];
    uint8_t enabled_bullets;

    bool last_bullet;
    bool req_bullet;

    int bounced;
    Graphics::Line spark;

    const Joysticks::Player &input;

    Player(Joysticks::Player &input);

    void step();

    void draw();
};
