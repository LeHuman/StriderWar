#pragma once
#include <stddef.h>
#include <stdint.h>

#include "interface.hpp"

namespace DOS {
namespace Input {
    namespace Joystick {
        static const size_t REGISTER_PORT = 0x201;
        static const size_t MAX_COUNTS = 16;
        static const size_t LUT_SIZE = MAX_COUNTS;

        static const int16_t RANGE_MAX = 50;
        static const int16_t RANGE_MIN = -50;

        struct AxisCalibration {
            int16_t min;
            int16_t max;
            int16_t lut[LUT_SIZE];

            AxisCalibration() : min(9999), max(0) {};
        };

        struct Calibration {
            AxisCalibration x;
            AxisCalibration y;
        };

        struct State : Interface {
            int16_t rawX;
            int16_t rawY;
            Calibration cal;

            inline void State::set_calibration(const int16_t *xLUT, const int16_t *yLUT) {
                for (int i = 0; i < LUT_SIZE; ++i) {
                    cal.x.lut[i] = xLUT[i];
                    cal.y.lut[i] = yLUT[i];
                }
            }

            inline void State::get_calibration(const int16_t **xLUT, const int16_t **yLUT) {
                *xLUT = cal.x.lut;
                *yLUT = cal.y.lut;
            }

            void calibrate_step();

            State() : rawX(0), rawY(0), cal() {};
        };

        void initialize(bool invertY);
        void update();

        extern State playerA;
        extern State playerB;

    } // namespace Joystick
} // namespace Input
} // namespace DOS
