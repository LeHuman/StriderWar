#pragma once

#include <stdlib.h>

#include "condition.hpp"
#include "debug.hpp"
#include "strider.hpp"

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

    void damage_roll();

    void step_damage();

    inline void reset() {
        condition.cockpit = Condition::HIGH;
        condition.body = Condition::HIGH;
        condition.thruster.left = Condition::HIGH;
        condition.thruster.right = Condition::HIGH;

        breach = false;
        pressure = MAX_PRESSURE;
        inferno = 0;
        lost_cycles = 0;
        update_cycle = 0;

        step_damage();
    }

    Ship() {
        reset();
    }
};
