#include "dos/input/joystick.hpp"

#include <conio.h>
#include <dos.h>
#include <stdio.h>

namespace DOS {
namespace Input {
    namespace Joystick {

        State playerA;
        State playerB;

        void initialize() {
            playerA = State();
            playerB = State();
        }

        void buildLUT(AxisCalibration &cal, bool invert = false) {
            if (cal.max == cal.min)
                return;

            const int iv = invert ? -1 : 1;

            for (int i = 0; i < LUT_SIZE; ++i) {
                int val = (i - cal.min) * (RANGE_MAX - RANGE_MIN) / (cal.max - cal.min) - RANGE_MAX;
                if (val < RANGE_MIN)
                    val = RANGE_MIN;
                if (val > RANGE_MAX)
                    val = RANGE_MAX;
                cal.lut[i] = val * iv;
            }
            cal.lut[cal.max / 2] = 0;
        }

        void State::calibrate_step() {
            bool update_x = false;
            bool update_y = false;

            if (rawX < cal.x.min) {
                cal.x.min = rawX;
                update_x = true;
            } else if (rawX > cal.x.max) {
                cal.x.max = rawX;
                update_x = true;
            }

            if (rawY < cal.y.min) {
                cal.y.min = rawY;
                update_y = true;
            } else if (rawY > cal.y.max) {
                cal.y.max = rawY;
                update_y = true;
            }

            if (update_x) {
                buildLUT(cal.x);
            }

            if (update_y) {
                buildLUT(cal.y, true);
            }
        }

        void update() {
            playerA.rawX = 0;
            playerA.rawY = 0;
            playerB.rawX = 0;
            playerB.rawY = 0;

            outp(REGISTER_PORT, 0);
            unsigned input = inp(REGISTER_PORT);

            playerA.fire = (input & (1 << 4)) == 0;
            playerA.alt = (input & (1 << 5)) == 0;
            playerB.fire = (input & (1 << 6)) == 0;
            playerB.alt = (input & (1 << 7)) == 0;

            unsigned done = 0x0F; // Bits 0-3 = X1, Y1, X2, Y2

            for (int i = 0; (i < MAX_COUNTS) && done; ++i) {
                if ((done & (1 << 0)) && (input & (1 << 0))) {
                    playerA.rawX++;
                } else if (done & (1 << 0)) {
                    done &= ~(1 << 0);
                }

                if ((done & (1 << 1)) && (input & (1 << 1))) {
                    playerA.rawY++;
                } else if (done & (1 << 1)) {
                    done &= ~(1 << 1);
                }

                if ((done & (1 << 2)) && (input & (1 << 2))) {
                    playerB.rawX++;
                } else if (done & (1 << 2)) {
                    done &= ~(1 << 2);
                }

                if ((done & (1 << 3)) && (input & (1 << 3))) {
                    playerB.rawY++;
                } else if (done & (1 << 3)) {
                    done &= ~(1 << 3);
                }

                input = inp(REGISTER_PORT);
            }

            playerA.x = playerA.cal.x.lut[playerA.rawX];
            playerA.y = playerA.cal.y.lut[playerA.rawY];
            playerB.x = playerB.cal.x.lut[playerB.rawX];
            playerB.y = playerB.cal.y.lut[playerB.rawY];
        }

    } // namespace Joystick
} // namespace Input
} // namespace DOS
