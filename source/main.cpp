// #include <cmath>
// #include <iostream>

#include <conio.h>
#include <dos.h>
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>

// #include "fixed.hpp"
// #include "fixed_math.hpp"
// #include "graphics.hpp"
// #include "i86.h"
#include "joystick.hpp"
#include "player.hpp"

int main() {
    Graphics::init();

    Joysticks joysticks(0x201);

    Player playerA(joysticks.playerA);
    Player playerB(joysticks.playerB);

    while (!kbhit()) {
        joysticks.update();

        playerA.step();
        playerB.step();

        playerA.draw();
        playerB.draw();
    }

    getch();
    return 0;
}
