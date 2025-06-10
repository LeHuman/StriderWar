#pragma once
#include <stdint.h>

namespace DOS {
namespace Time {

    class Timer {
    public:
        Timer();

        void beginFrame();           // Mark the beginning of a frame
        uint32_t elapsedMs();        // Get elapsed time in ms
        void waitForMs(uint32_t ms); // Busy-wait until N ms have passed

    private:
        uint32_t biosTicks();
        uint16_t readPIT();

        uint32_t startTicks;
        uint16_t startPit;
    };

} // namespace Time
} // namespace DOS
