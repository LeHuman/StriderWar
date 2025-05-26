#ifndef JOYSTICK_H
#define JOYSTICK_H

static const unsigned int MAX_COUNTS = 128;

class Joysticks {
    const unsigned port;

    enum Axis {
        _X1 = 1U << 0U,
        _Y1 = 1U << 1U,
        _X2 = 1U << 2U,
        _Y2 = 1U << 3U,
        _ALL = _X1 | _Y1 | _X2 | _Y2,
    };

public:
    int X1;
    int Y1;
    bool A1;
    bool B1;

    int X2;
    int Y2;
    bool A2;
    bool B2;

    enum Player {
        A,
        B
    };

    Joysticks(unsigned port) : port(port) {};
    void update();

};

#endif // JOYSTICK_H
