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
    DOS::Draw::Line spark;

    const DOS::Input::Interface &input;

    Player(DOS::Input::Interface &input);

    void step();

    void draw();
};
