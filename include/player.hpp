#pragma once

#include <dos/input.hpp>
#include <rand.hpp>
#include <stdlib.h>

#include "bullet.hpp"
#include "condition.hpp"
#include "debug.hpp"
#include "ship.hpp"
#include "strider.hpp"

extern bool player_situation_update;

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

        void acknowledge() {
            v = (status_e)(v >> 4);
        }

        inline const status_e get() {
            return v;
        }

        void set(const status_e &set) {
            if ((v << 4) != set) {
                v = set;
                player_situation_update = true;
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

    void step_situation() {
        switch (situation_cycle++ % 20) { // MAX_SITUATIONS?
            case 0:
                situation.ship.pilot.set(ship.condition.cockpit);
                break;
            case 1:
                situation.ship.body.set(ship.condition.body);
                break;
            case 2:
                situation.ship.thruster.left.set(ship.condition.thruster.left);
                break;
            case 3:
                situation.ship.thruster.right.set(ship.condition.thruster.right);
                break;
            case 4:
                situation.bullet[0].head.set(bullets[0].condition.payload);
                break;
            case 5:
                situation.bullet[0].body.set(bullets[0].condition.body);
                break;
            case 6:
                situation.bullet[0].boost.set(bullets[0].condition.booster);
                break;
            case 7:
                situation.bullet[1].head.set(bullets[1].condition.payload);
                break;
            case 8:
                situation.bullet[1].body.set(bullets[1].condition.body);
                break;
            case 9:
                situation.bullet[1].boost.set(bullets[1].condition.booster);
                break;
            case 10:
                situation.panel.ammo_low.set(Condition::disable_high((Condition::T)((bullets[0].condition.payload + bullets[1].condition.payload + 1) / 2)));
                break;
            case 11:
                situation.panel.auto_pilot.set(ship.auto_pilot_forced() ? P_SS_FAIL : P_SS_OFF);
                break;
            case 12:
                situation.panel.booster_damage.set(Condition::disable_high((Condition::T)((bullets[0].condition.booster + bullets[1].condition.booster + 1) / 2)));
                break;
            case 13:
                situation.panel.dead.set(ship.is_disabled() ? P_SS_FAIL : P_SS_OFF);
                break;
            case 14:
                situation.panel.fire.set(ship.inferno ? (ship.inferno > (ship.MAX_INFERNO / 2) ? P_SS_FAIL : P_SS_FAIR) : P_SS_OFF);
                break;
            case 15:
                situation.panel.fuel_low.set(Condition::disable_high((Condition::T)((bullets[0].condition.body + bullets[1].condition.body + 1) / 2)));
                break;
            case 16:
                situation.panel.hull_breach.set(ship.pressure < (ship.MAX_PRESSURE / 3) ? P_SS_FAIL : (ship.pressure < ((ship.MAX_PRESSURE * 2) / 3) ? P_SS_FAIR : (ship.pressure == ship.MAX_PRESSURE ? P_SS_OFF : P_SS_GOOD)));
                break;
            case 17:
                situation.panel.left_authority.set(ship.condition.thruster.left);
                break;
            case 18:
                situation.panel.right_authority.set(ship.condition.thruster.right);
                break;
            case 19:
                situation.panel.nuclear_meltdown.set(meltdown_cycle > ((MAX_MELTDOWN_CYCLES * 2) / 3) ? P_SS_FAIL : (meltdown_cycle > (MAX_MELTDOWN_CYCLES / 3) ? P_SS_FAIR : (meltdown_cycle == 0 ? P_SS_OFF : P_SS_GOOD)));
                break;
            // TODO: Update remaining situations
            default:
                // Not updated here
                break;
        }
    }

    void step();

    void draw() {
        if (bounced) {
            DOS::Draw::line(spark, bounced - 1);
            if (bounced == 4) {
                bounced = 1;
            } else {
                bounced = 0;
            }
        }

        ship.entity.draw();
        for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
            bullets[i].entity.draw();
        }
    }

    void damage(int hits) {
        damage_queue += hits;
    }
};
