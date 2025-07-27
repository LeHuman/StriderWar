#pragma once

#include <conio.h>
#include <dos.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

        extern State playerA;
        extern State playerB;

        extern bool invertY;

        inline void initialize(bool invertY) {
            Joystick::invertY = invertY;
            playerA = State();
            playerB = State();
        }

        inline bool load_calibration(const char *calibration_file) {
            FILE *f = fopen(calibration_file, "rb");
            if (f) {
                uint8_t *dataA = (uint8_t *)&playerA.cal;
                uint8_t *dataB = (uint8_t *)&playerB.cal;
                const size_t data_size = sizeof(playerA.cal);
                size_t read = fread(dataA, 1, data_size, f);
                read += fread(dataB, 1, data_size, f);
                fclose(f);
                return read == (data_size * 2);
            }
            // debug::serial_printf("Could not open %s\n", calibration_file);
            return false;
        }

        inline bool save_calibration(const char *calibration_file) {
            const uint8_t *dataA = (uint8_t *)&playerA.cal;
            const uint8_t *dataB = (uint8_t *)&playerB.cal;
            const size_t data_size = sizeof(playerA.cal);

            FILE *f = fopen(calibration_file, "wb");
            if (!f) {
                // debug::serial_printf("Could not open %s\n", calibration_file);
                return false;
            }

            size_t written = fwrite(dataA, 1, data_size, f);
            written += fwrite(dataB, 1, data_size, f);
            if (written != (data_size * 2)) {
                // debug::serial_printf("File write failed for %s\n", calibration_file);
                fclose(f);
                return false;
            }

            fclose(f);
            return true;
        }

        inline void update() {
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

        extern State playerA;
        extern State playerB;

    } // namespace Joystick
} // namespace Input
} // namespace DOS
