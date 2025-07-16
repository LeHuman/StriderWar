#pragma once

#include <dos/input.hpp>
#include <dos/rand.hpp>
#include <stdlib.h>

#include "debug.hpp"
#include "strider.hpp"

namespace Condition {
enum T {
    DISABLED,
    LOW,
    MID,
    HIGH,
};

// FIXME: don't allow 'looping' back to high
Condition::T disable_high(T condition) {
    if (condition == DISABLED) {
        return HIGH;
    } else if (condition == HIGH) {
        return DISABLED;
    }
    return condition;
}

void damage(T &condition) {
    condition = condition > 0 ? (T)(condition - 1) : Condition::DISABLED;
}
void damage(T *condition) {
    *condition = *condition > 0 ? (T)(*condition - 1) : Condition::DISABLED;
}
} // namespace Condition

struct Bullet {
    Strider entity;

    static const size_t MAX_DELAY = 100;

    bool loaded;
    size_t delay;

    struct
    {
        int speed;
        int damage;
    } mult;

    struct C {
        Condition::T payload;
        Condition::T body;
        Condition::T booster;
    } condition;

    inline bool is_disabled() {
        return condition.body == Condition::DISABLED;
    }

    inline void fired() {
        loaded = false;
    }

    void step_loading(size_t added_delay = 0) {
        if (!loaded && (--delay == 0)) {
            loaded = true;
            delay = MAX_DELAY + added_delay;
        }
    }

    void step_damage() {
        mult.speed = (condition.booster + 1) * 10 / 4;
        mult.damage = condition.payload + 1;
    }

    Bullet();
};

struct Ship {
    Strider entity;

    static const size_t MAX_UPDATE_CYCLE = 10;
    static const size_t MAX_LOST_CYCLES = 10;
    static const int MAX_PRESSURE = 100;
    static const int MAX_INFERNO = 100;
    static const int MAX_THRUST_BOOST = 100;

    bool breach;
    int pressure;
    int inferno;
    size_t lost_cycles;
    size_t update_cycle;

    struct
    {
        int left_turn;
        int right_turn;
    } mult;

    struct C {
        Condition::T cockpit;
        Condition::T body;
        struct
        {
            Condition::T left;
            Condition::T right;
        } thruster;
    } condition;

    void trigger_breach() {
        if (pressure > 0) {
            breach = true;
        }
    }

    void trigger_fire() {
        if (pressure > 0) {
            inferno += 1;
        } else {
            inferno = 0;
        }
    }

    inline bool auto_pilot_forced() {
        return condition.cockpit == Condition::DISABLED;
    }

    inline bool is_disabled() {
        return condition.body == Condition::DISABLED;
    }

    void damage_roll() {
        switch (DOS::rand::get(16)) {
            case 0:
            case 1:
            case 15:
                debug::serial_print("fire\n");
                trigger_fire();
                break;
            case 3:
                debug::serial_print("breach\n");
                trigger_breach();
                break;
            case 5:
                Condition::damage(condition.thruster.left);
                break;
            case 6:
                Condition::damage(condition.thruster.right);
                break;
            case 8:
                Condition::damage(condition.cockpit);
                break;
            case 11:
                Condition::damage(condition.body);
                break;
            default:
                // no event
                break;
        }
    }

    void step_damage() {
        if (update_cycle++ % MAX_UPDATE_CYCLE != 0) {
            return;
        }

        mult.left_turn = ((condition.thruster.right + 1) * MAX_THRUST_BOOST) / 4;
        mult.right_turn = ((condition.thruster.left + 1) * MAX_THRUST_BOOST) / 4;

        const int lost_cond = condition.thruster.left + condition.thruster.right + condition.cockpit;
        if (lost_cycles) {
            entity.pulse(entity.vy / -2, entity.vx / -2);
            --lost_cycles;
        } else if (lost_cond != 9 && (DOS::rand::get(2 + lost_cond) == 0)) {
            debug::serial_print("lost");
            lost_cycles = MAX_LOST_CYCLES;
        }

        if (breach) {
            pressure--;
            // Three 1/5 rolls to cause damage
            if ((pressure % (MAX_INFERNO / 4) == 0) && (DOS::rand::get(4) == 0)) {
                damage_roll();
            }
            breach = pressure > 0;
        }

        if (inferno) {
            trigger_fire();
        }

        if (inferno >= MAX_INFERNO) {
            damage_roll();
            inferno = 0;
        }
    }

    Ship();
};

struct Player {
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

        void set(const Condition::T &set) {
            return this->set((status_e)(1 << (set + 4)));
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
    int damage_queue;
    Ship ship;
    Bullet bullets[MAX_BULLETS];
    size_t enabled_bullets;
    size_t bullet_delay;
    situation_t &situation;
    static const size_t MAX_SITUATIONS = sizeof(situation_t) / sizeof(status);
    size_t situation_cycle;

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
