#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef interrupt
// FOR IDE LINTING
#define interrupt
#endif

namespace Time {
namespace ISR {

    typedef void interrupt (*Callback)();

    void initialize(Callback callback, int hertz);
    void bios_call();
    void shutdown();

} // namespace ISR
} // namespace Time
