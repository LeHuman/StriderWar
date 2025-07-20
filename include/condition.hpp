#pragma once

#include <stdlib.h>

namespace Condition {
enum T {
    DISABLED,
    LOW,
    MID,
    HIGH,
};

T disable_high(T condition);

void damage(T *condition);
} // namespace Condition
