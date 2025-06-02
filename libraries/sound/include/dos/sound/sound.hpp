#pragma once

class Sound {
public:
    static void init();
    static void tone(int channel, int frequency, int volume); // 0-2
    static void noise(int type, int volume);                  // type: 0-3
    static void silence();                                    // silence all
};
