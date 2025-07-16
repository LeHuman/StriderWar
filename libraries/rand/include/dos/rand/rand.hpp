#pragma once

#include <stddef.h>
#include <stdint.h>

namespace DOS {
namespace rand {

    const size_t RANDOM_LUT_SIZE = 127;

    int get();

    int get(int low, int high);

    int get(int high);

} // namespace rand
} // namespace DOS
