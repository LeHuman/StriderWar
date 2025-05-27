#pragma once

#include <stdlib.h>

#include "fixed.hpp"
#include "fixed_math.hpp"
#include "graphics.hpp"
#include "joysticks.hpp"
#include "strider.hpp"
#include "world.hpp"

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
