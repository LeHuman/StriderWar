#include "rand/rand.hpp"

#include "rand/rand_lut.hpp"

namespace random {

static size_t rand_i = 0;

rand_t get() {
    rand_t val = random_ints[rand_i++ % RANDOM_LUT_SIZE];
    return val;
}

rand_t get(int high) {
    rand_t val = get();
    // Map signed to unsigned
    urand_t uval = val ^ 0x8000;
    return uval % (high + 1);
}

} // namespace random
