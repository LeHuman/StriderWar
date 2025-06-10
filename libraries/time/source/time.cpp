#include "dos/time.hpp"

#include <conio.h>
#include <dos.h>

#define BIOS_TICK_SEG 0x40
#define BIOS_TICK_OFS 0x6C

#define PIT_CMD 0x43
#define PIT_CH0 0x40

namespace DOS {
namespace Time {
    // Read BIOS 18.2 Hz tick count (wraps every ~24 hours)
    uint32_t Timer::biosTicks() {
        return *(volatile uint32_t far *)MK_FP(BIOS_TICK_SEG, BIOS_TICK_OFS);
    }

    // Read PIT channel 0 (counts down from 0xFFFF)
    uint16_t Timer::readPIT() {
        outp(PIT_CMD, 0x00); // Latch channel 0
        uint8_t lo = inp(PIT_CH0);
        uint8_t hi = inp(PIT_CH0);
        return (hi << 8) | lo;
    }

    Timer::Timer() {
        beginFrame();
    }

    void Timer::beginFrame() {
        startTicks = biosTicks();
        startPit = readPIT();
    }

    // Returns elapsed time since `beginFrame()` in milliseconds
    uint32_t Timer::elapsedMs() {
        uint32_t nowTicks = biosTicks();
        uint16_t nowPit = readPIT();

        uint32_t tickDiff = nowTicks - startTicks;
        uint16_t pitDiff = (startPit - nowPit) & 0xFFFF;

        // Convert BIOS ticks (each ≈ 55ms)
        // PIT ticks are ~0.84μs per count → ~1.193MHz → 65536 ~ 54.9 ms
        uint32_t ms = tickDiff * 55 + ((pitDiff * 1000UL) / 65536UL);
        return ms;
    }

    void Timer::waitForMs(uint32_t ms) {
        while (elapsedMs() < ms) {
            // idle loop — optionally call update() here
        }
    }
} // namespace Time
} // namespace DOS
