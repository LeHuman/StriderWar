#pragma once

#include <dos/input.hpp>
#include <stdlib.h>

#include "strider.hpp"

namespace Condition {
enum T {
    DISABLED,
    LOW,
    MID,
    HIGH,
};
}

struct Bullet {
    Strider entity;

    struct
    {
        int speed;
        int damage;
    } mult;

    struct
    {
        Condition::T payload;
        Condition::T body;
        Condition::T booster;
    } condition;

    inline bool is_disabled() {
        return condition.body == Condition::DISABLED;
    }

    void update() {
        mult.speed = (condition.booster + 1) * 10 / 4;
        mult.damage = condition.payload + 1;
    }

    // Bullet() : mult{10, 4}, condition{HIGH, HIGH, HIGH} {}
};

struct Ship {
    Strider entity;

    bool breach;
    int pressure;
    int inferno;

    struct
    {
        int left_turn;
        int right_turn;
    } mult;

    struct {
        Condition::T cockpit;
        Condition::T body;
        struct
        {
            Condition::T left;
            Condition::T right;
        } thruster;
    } condition;

    void trigger_breach() {
        breach = true;
    }

    void trigger_fire() {
        if (pressure > 0) {
            inferno = 100;
        }
    }

    inline bool auto_pilot_forced() {
        return condition.cockpit == Condition::DISABLED;
    }

    inline bool is_disabled() {
        return condition.body == Condition::DISABLED;
    }

    void damage_roll() {
    }

    void update() {
        mult.left_turn = (condition.thruster.right + 1) * 10 / 4;
        mult.right_turn = (condition.thruster.left + 1) * 10 / 4;
    }
};

struct Player {
    static const size_t MAX_BULLETS = 2;
    Condition::T *blast_field[(MAX_BULLETS * 3)];

    enum status_e {
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

    struct status {
        status_e v;

        void acknowledge() {
            v = (status_e)(v >> 4);
        }

        inline const status_e get() {
            return v;
        }

        void set(const status_e &set) {
            if ((v << 4) != set) {
                v = set;
            }
        }
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
