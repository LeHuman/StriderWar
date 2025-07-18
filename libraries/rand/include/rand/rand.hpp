#pragma once

#include <stddef.h>
#include <stdint.h>

namespace random {

typedef int16_t rand_t;

const size_t RANDOM_LUT_SIZE = 127;

rand_t get();

rand_t get(int low, int high);

rand_t get(int high);

} // namespace random
