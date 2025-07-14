#pragma once

#include <dos/input.hpp>
#include <stdlib.h>

#include "strider.hpp"

struct Player {
    static const size_t MAX_BULLETS = 2;

    enum s_status {
        P_SS_INVALID = 0,
        P_SS_OFF_DONE = 1 << 0,
        P_SS_GOOD_DONE = 1 << 1,
        P_SS_FAIR_DONE = 1 << 2,
        P_SS_FAIL_DONE = 1 << 3,
        P_SS_OFF = 1 << 4,
        P_SS_GOOD = 1 << 5,
        P_SS_FAIR = 1 << 6,
        P_SS_FAIL = 1 << 7,
    };

    struct situation_t {
        struct {
            s_status pilot;
            s_status body;
            struct {
                s_status left;
                s_status right;
            } thruster;
        } ship;

        struct {
            s_status indicator;
            s_status head;
            s_status body;
            s_status boost;
        } bullet[MAX_BULLETS];

        struct {
            s_status gun_ready;
            s_status left_authority;
            s_status low_speed;
            s_status right_authority;
            s_status ammo_low;
            s_status auto_pilot;
            s_status fuel_low;
            s_status fire;
            s_status booster_damage;
            s_status hull_breach;
            s_status nuclear_meltdown;
            s_status dead;
        } panel;
    };

    int id;
    Strider ship;
    Strider bullets[MAX_BULLETS];
    uint8_t enabled_bullets;
    situation_t &situation;

    bool last_bullet;
    bool req_bullet;

    int bounced;
    DOS::Draw::Line spark;

    const DOS::Input::Interface &input;

    Player(DOS::Input::Interface &input, situation_t &situation_mem);

    inline bool valid() {
        return id != -1;
    }

    void step();

    void draw();

    void damage();
};
