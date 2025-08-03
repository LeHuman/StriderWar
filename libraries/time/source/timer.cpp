#include <conio.h>
#include <dos.h>
#include <i86.h>

#include "time.hpp"

#define PIT_HZ 1193182UL // PIT base frequency

#define PIT_RELOAD ((uint16_t)(PIT_HZ / TICK_RATE_HZ))

// Original interrupt vector (in case you want to restore or chain)
static void interrupt (*old_timer_isr)() = 0;

namespace Time {
namespace ISR {

    void bios_call() {
        old_timer_isr();
    }

    // hertz: 60, 100, 500
    void initialize(Callback callback, int hertz) {
        // TODO: Move all DOS isr handling to same place
        // Save old ISR
        old_timer_isr = _dos_getvect(0x08);
        _dos_setvect(0x08, callback);

        const int pit_reload = 1193182 / hertz;

        // Set PIT channel 0 mode 3 (square wave generator)
        outp(0x43, 0x36);              // Channel 0, LSB+MSB, mode 3
        outp(0x40, pit_reload & 0xFF); // LSB
        outp(0x40, pit_reload >> 8);   // MSB
    }

    void shutdown() {
        // Restore original ISR
        _dos_setvect(0x08, old_timer_isr);
    }

} // namespace ISR
} // namespace Time
