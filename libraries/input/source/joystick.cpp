#include "dos/input/joystick.hpp"

#include <conio.h>
#include <dos.h>
#include <stdio.h>

namespace DOS {
namespace Input {
    namespace Joystick {

        State playerA;
        State playerB;

        bool invertY = false;

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
                buildLUT(cal.y, invertY);
            }
        }

    } // namespace Joystick
} // namespace Input
} // namespace DOS
