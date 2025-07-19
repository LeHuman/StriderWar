#include "ship.hpp"

#include <rand.hpp>

Ship::Ship() : breach(false), pressure(MAX_PRESSURE), inferno(0), update_cycle(0) {
    condition.cockpit = Condition::HIGH;
    condition.body = Condition::HIGH;
    condition.thruster.left = Condition::HIGH;
    condition.thruster.right = Condition::HIGH;

    step_damage();
}

void Ship::trigger_breach() {
    if (pressure > 0) {
        breach = true;
    }
}

void Ship::trigger_fire() {
    if (pressure > 0) {
        inferno += 1;
    } else {
        inferno = 0;
    }
}

void Ship::damage_roll() {
    switch (random::get(16)) {
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

void Ship::step_damage() {
    if (update_cycle++ % MAX_UPDATE_CYCLE != 0) {
        return;
    }

    mult.left_turn = ((condition.thruster.right + 1) * MAX_THRUST_BOOST) / 4;
    mult.right_turn = ((condition.thruster.left + 1) * MAX_THRUST_BOOST) / 4;

    const int lost_cond = condition.thruster.left + condition.thruster.right + condition.cockpit;
    if (lost_cycles) {
        entity.pulse(entity.vy / -2, entity.vx / -2);
        --lost_cycles;
    } else if (lost_cond != 9 && (random::get(2 + lost_cond) == 0)) {
        debug::serial_print("lost");
        lost_cycles = MAX_LOST_CYCLES;
    }

    if (breach) {
        pressure--;
        // Three 1/5 rolls to cause damage
        if ((pressure % (MAX_INFERNO / 4) == 0) && (random::get(4) == 0)) {
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
