#pragma once

#include <dos/input.hpp>
#include <stdlib.h>

#include "strider.hpp"

struct Player {
    static const size_t MAX_BULLETS = 2;
    int id;
    Strider ship;
    Strider bullets[MAX_BULLETS];
    uint8_t enabled_bullets;

    bool last_bullet;
    bool req_bullet;

    int bounced;
    DOS::Draw::Line spark;

    const DOS::Input::Interface &input;

    Player(DOS::Input::Interface &input);

    inline bool valid() {
        return id != -1;
    }

    void step();

    void draw();
};
