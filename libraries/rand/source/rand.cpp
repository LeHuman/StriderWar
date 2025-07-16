#include "dos/rand/rand.hpp"

#include "dos/rand/rand_lut.hpp"

namespace DOS {
namespace rand {

    static size_t rand_i = 0;

    int get() {
        int16_t val = random_ints[rand_i++ % RANDOM_LUT_SIZE];
        return val;
    }

    int get(int high) {
        int16_t val = get();
        // Map signed to unsigned
        uint16_t uval = val ^ 0x8000;
        return uval % (high + 1);
    }

    int get(int low, int high) {
        int range = (high + 1) - low;
        return get(range - 1) + low;
    }

} // namespace rand
} // namespace DOS
