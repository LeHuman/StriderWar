#pragma once

namespace DOS {
namespace Sound {
    void initialize();
    void tone(int channel, int frequency, int volume); // 0-2
    void noise(int type, int volume);                  // type: 0-3
    void silence();                                    // silence all
    void play(
        int freq0, int vol0,
        int freq1, int vol1,
        int freq2, int vol2,
        int noiseType, int noiseVol, int spkr_freq);
} // namespace Sound
} // namespace DOS
