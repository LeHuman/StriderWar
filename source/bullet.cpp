#include "bullet.hpp"

void Bullet::reset() {
    condition.payload = Condition::HIGH;
    condition.body = Condition::HIGH;
    condition.booster = Condition::HIGH;

    step_damage();
}
