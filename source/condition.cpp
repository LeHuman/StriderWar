#include "condition.hpp"

namespace Condition {

T disable_high(T condition) {
    // FIXME: don't allow 'looping' back to high
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
