#pragma once

#include <stdlib.h>

#include "condition.hpp"
#include "debug.hpp"
#include "strider.hpp"

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

    void reset();

    Bullet() : loaded(true), delay(MAX_DELAY) {
        reset();
    }
};
