#pragma once

#include <dos/input.hpp>
#include <rand.hpp>
#include <stdlib.h>

#include "condition.hpp"
#include "debug.hpp"
#include "strider.hpp"
#include "bullet.hpp"
#include "ship.hpp"

struct Player {
    static const size_t MAX_MELTDOWN_CYCLES = 1000;
    static const size_t MAX_BULLETS = 2;
    static const size_t CONDITIONS = ((sizeof(Bullet::C) / sizeof(Condition::T)) * MAX_BULLETS) + (sizeof(Ship::C) / sizeof(Condition::T));
    Condition::T *blast_field[CONDITIONS];

    enum status_e {
        P_SS_INVALID = 0,
        P_SS_OFF_DONE = 1 << 0,
        P_SS_FAIL_DONE = 1 << 1,
        P_SS_FAIR_DONE = 1 << 2,
        P_SS_GOOD_DONE = 1 << 3,
        P_SS_OFF = 1 << 4,
        P_SS_FAIL = 1 << 5,
        P_SS_FAIR = 1 << 6,
        P_SS_GOOD = 1 << 7,
    };

    struct status {
        status_e v;

        void acknowledge();

        inline const status_e get() {
            return v;
        }

        void set(const status_e &set);
        void set(const Condition::T &set);
    };

    struct situation_t {
        struct {
            status pilot;
            status body;
            struct {
                status left;
                status right;
            } thruster;
        } ship;

        struct {
            status indicator;
            status head;
            status body;
            status boost;
        } bullet[MAX_BULLETS];

        struct {
            status gun_ready;
            status left_authority;
            status low_speed;
            status right_authority;
            status ammo_low;
            status auto_pilot;
            status fuel_low;
            status fire;
            status booster_damage;
            status hull_breach;
            status nuclear_meltdown;
            status dead;
        } panel;
    };

    int id;
    int damage_queue;
    Ship ship;
    Bullet bullets[MAX_BULLETS];
    size_t enabled_bullets;
    size_t bullet_delay;
    situation_t &situation;
    static const size_t MAX_SITUATIONS = sizeof(situation_t) / sizeof(status);
    size_t situation_cycle;
    size_t meltdown_cycle;

    bool last_bullet;
    bool req_bullet;

    int bounced;
    DOS::Draw::Line spark;

    const DOS::Input::Interface &input;

    Player(DOS::Input::Interface &input, situation_t &situation_mem);

    inline bool valid() {
        return id != -1;
    }

    void step_damage();

    void step_situation();

    void step();

    void draw();

    void damage(int hits);
};
