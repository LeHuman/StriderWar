#include "bullet.hpp"

Bullet::Bullet() : loaded(true), delay(MAX_DELAY) {
    condition.payload = Condition::HIGH;
    condition.body = Condition::HIGH;
    condition.booster = Condition::HIGH;

    step_damage();
}

void Bullet::step_loading(size_t added_delay) {
    if (!loaded && (--delay == 0)) {
        loaded = true;
        delay = MAX_DELAY + added_delay;
    }
}

void Bullet::step_damage() {
    mult.speed = (condition.booster + 1) * 10 / 4;
    mult.damage = condition.payload + 1;
}
