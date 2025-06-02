#include "dos/sound/sound.hpp"

#include <conio.h>
#include <dos.h>

#define PSG_PORT 0xC0

void Sound::init() {
    silence();
}

void Sound::tone(int channel, int frequency, int volume) {
    if (channel < 0 || channel > 2 || frequency <= 0)
        return;

    // The PSG uses a 3.58 MHz clock, divide by 32 = ~111860 Hz input
    // Tone freq = Clock / (32 * N), so N = Clock / (32 * F)
    unsigned int N = 111860 / frequency;
    if (N > 0x3FF)
        N = 0x3FF;

    unsigned char coarse = (N >> 4) & 0x3F;
    unsigned char fine = N & 0x0F;

    // Send tone latch byte
    outp(PSG_PORT, 0x80 | (channel << 5) | fine);            // Latch + fine
    outp(PSG_PORT, coarse);                                  // Coarse bits
    outp(PSG_PORT, 0x90 | (channel << 5) | (volume & 0x0F)); // Volume
}

void Sound::noise(int type, int volume) {
    // Type: 0-3 for periodic/random and frequency control
    outp(PSG_PORT, 0xE0 | (type & 0x07));
    outp(PSG_PORT, 0xF0 | (volume & 0x0F));
}

void Sound::silence() {
    for (int ch = 0; ch < 3; ++ch) {
        outp(PSG_PORT, 0x90 | (ch << 5) | 0x0F); // Max attenuation (silent)
    }
    outp(PSG_PORT, 0xF0 | 0x0F);                 // Silence noise
}
