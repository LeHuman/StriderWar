#include "joystick.hpp"

#include <iostream>

#include <conio.h>
#include <dos.h>

void Joysticks::update() {
    X1 = 0;
    Y1 = 0;
    X2 = 0;
    Y2 = 0;

    unsigned output = outp(port, 0U);
    unsigned input = inp(port);
    unsigned done = _ALL;

    A1 = (input & 32U) == 0;
    B1 = (input & 16U) == 0;
    A2 = (input & 128U) == 0;
    B2 = (input & 64U) == 0;

    for (unsigned int i = 0; (i < MAX_COUNTS) && done; ++i) {
        // X1 += done &= input & _X1
        if (input & _X1 & done) {
            ++X1;
        } else {
            done &= ~_X1;
        }

        if (input & _Y1 & done) {
            ++Y1;
        } else {
            done &= ~_Y1;
        }

        if (input & _X2 & done) {
            ++X2;
        } else {
            done &= ~_X2;
        }

        if (input & _Y2 & done) {
            ++Y2;
        } else {
            done &= ~_Y2;
        }

        input = inp(port);
    }
}
