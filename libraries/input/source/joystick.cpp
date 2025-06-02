#include "dos/input/joystick.hpp"

#include <conio.h>
#include <dos.h>

enum PortBit {
    X1 = 1U << 0U,
    Y1 = 1U << 1U,
    X2 = 1U << 2U,
    Y2 = 1U << 3U,
    A1 = 1U << 4U,
    B1 = 1U << 5U,
    A2 = 1U << 6U,
    B2 = 1U << 7U,
    ALL_AXIS = X1 | Y1 | X2 | Y2,
};

void Joysticks::update() {
    playerA.x = 0;
    playerA.y = 0;
    playerB.x = 0;
    playerB.y = 0;

    outp(port, 0U);
    unsigned input = inp(port);
    unsigned done = ALL_AXIS;

    playerA.fire = (input & A1) == 0;
    playerA.alt = (input & B1) == 0;

    playerB.fire = (input & A2) == 0;
    playerB.alt = (input & B2) == 0;

    for (unsigned int i = 0; (i < MAX_COUNTS) && done; ++i) {
        if (input & X1 & done) {
            ++playerA.x;
        } else if (X1 & done) {
            done &= ~X1;
        }

        if (input & Y1 & done) {
            ++playerA.y;
        } else if (Y1 & done) {
            done &= ~Y1;
        }

        if (input & X2 & done) {
            ++playerB.x;
        } else if (X2 & done) {
            done &= ~X2;
        }

        if (input & Y2 & done) {
            ++playerB.y;
        } else if (Y2 & done) {
            done &= ~Y2;
        }

        input = inp(port);
    }
}
