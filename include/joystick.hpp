#ifndef JOYSTICK_H
#define JOYSTICK_H

class Joysticks {
    const unsigned port;

public:
    static const unsigned int MAX_COUNTS = 128U;

    struct Player {
        int x;
        int y;
        bool alt;
        bool fire;
    };

    Player playerA;
    Player playerB;

    Joysticks(unsigned port) : port(port) {};
    void update();
};

#endif // JOYSTICK_H
