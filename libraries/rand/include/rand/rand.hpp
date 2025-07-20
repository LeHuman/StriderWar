#pragma once

#include <stddef.h>
#include <stdint.h>

namespace random {

typedef int16_t rand_t;
typedef uint16_t urand_t;

const size_t RANDOM_LUT_SIZE = 127;

rand_t get();

rand_t get(int high);

rand_t get(int low, int high) {
    int range = (high + 1) - low;
    return get(range - 1) + low;
}

} // namespace random
