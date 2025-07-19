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

    void step_loading(size_t added_delay = 0);
    void step_damage();

    Bullet();
};
