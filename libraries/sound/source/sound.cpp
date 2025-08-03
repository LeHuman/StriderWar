#include "dos/sound/sound.hpp"

#include <conio.h>
#include <dos.h>
#include <i86.h>

#define PSG_PORT 0xC0
#define PPI_PORTB 0x61
#define SPEAKER_CTRL 0x61
#define PIT_CHANNEL2 0x42

namespace DOS {
namespace Sound {

    void initialize() {
        unsigned char val = inp(PPI_PORTB);
        val |= (1 << 5); // PSG enable
        val |= (1 << 6); // amplifier enable
        outp(PPI_PORTB, val);
        silence();
    }

    void tone(int channel, int frequency, int volume) {
        if (channel < 0 || channel > 2) {
            return;
        }

        if (frequency <= 0) {
            outp(PSG_PORT, 0x90 | (channel << 5) | 0x0F);
            return;
        }

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

    void noise(int type, int volume) {
        // Type: 0-3 for periodic/random and frequency control
        outp(PSG_PORT, 0xE0 | (type & 0x07));
        outp(PSG_PORT, 0xF0 | (volume & 0x0F));
    }

    void silence() {
        for (int ch = 0; ch < 3; ++ch) {
            outp(PSG_PORT, 0x90 | (ch << 5) | 0x0F); // Max attenuation (silent)
        }
        outp(PSG_PORT, 0xF0 | 0x0F);                 // Silence noise
        nosound();
    }

    void play(
        int freq0, int vol0,
        int freq1, int vol1,
        int freq2, int vol2,
        int noiseType, int noiseVol, int spkr_freq) {
        tone(0, freq0, vol0);
        tone(1, freq1, vol1);
        tone(2, freq2, vol2);
        noise(noiseType, noiseVol);
        sound(spkr_freq);
    }

} // namespace Sound
} // namespace DOS
