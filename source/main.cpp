#include <cmath>
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cube.hpp"
#include "fixed/fixed.hpp"
#include "fixed/fixed_math.hpp"
#include "graphics.hpp"
#include "i86.h"
#include "joystick.hpp"
#include "sound.hpp"

static const float X_MIN = 0.0f;
static const float X_MAX = 640.0f / 2.0f;
static const float Y_MIN = 0.0f;
static const float Y_MAX = 200.0f;

int main() {
    Graphics::setVideoMode(0x0D); // 320x200x16 color mode

    Cube cube;
    float angleX = 10.0f, angleY = 10.0f;

    Joysticks joysticks(0x201);

    int count = 0;

    char pos_str[100];

    sound(440);
    // Sound::init();
    delay(100);
    nosound();
    // Sound::tone(0, 440, 5);
    // Sound::tone(1, 250, 0);
    // Sound::tone(2, 300, 2);
    // delay(500);
    // Sound::silence();

    const float gravity = 0.185f;
    const float friction = 0.98f;
    const float bounce = -0.86f;
    const float tail = 2.0f;

    const float bullet_speed = 2.0f;
    const float bullet_tail = 0.5f;

    const float spark_dist = 5.0f;
    const float spark_tail = -10.0f;
    const float spark_deflect_rad = 0.5235988;

    float last[3][4] = {0};

    float x = X_MAX / 2.0f;
    float y = Y_MIN / 2.0f;
    float vx = 20.0f;
    float vy = 10.0f;

    float bx = 0.0f;
    float by = 0.0f;
    float bvx = 0.0f;
    float bvy = 0.0f;

    unsigned int bullet_c = 0;
    unsigned int bullet_cmax = 0;

    bool bullet = false;
    bool bounceX = false;
    bool bounceY = false;

    while (!kbhit()) {
        joysticks.update();
        // std::cout << joysticks.X1 << ", " << joysticks.Y1 << ", " << joysticks.X2 << ", " << joysticks.Y2 << '\n';

        // int joyX1 = Joysticks::readAxis(0);
        // int joyY1 = Joysticks::readAxis(1);
        // int joyX2 = Joysticks::readAxis(2);
        // int joyY2 = Joysticks::readAxis(3);

        // std::cout << joyX1 << '\n';

        // std::cout.flush();

        // angleX += (joysticks.X1 - 3) / 8.0f;
        // angleY += (joysticks.Y1 - 3) / 8.0f;

        vy += gravity;

        angleX = (float)(joysticks.X1 - 3.0f) / 4.0f;
        angleY = (float)(joysticks.Y1 - 3.0f) / 4.0f;

        if (bounceX || bounceY) {
            Graphics::line(last[1][0], last[1][1], last[1][2], last[1][3], 0);
            Graphics::line(last[2][0], last[2][1], last[2][2], last[2][3], 0);
            bounceX = false;
            bounceY = false;
        }

        if (bullet) {
            if (((bullet_c % bullet_cmax) == 0)) {
                Graphics::line(bx, by, bx - (bvx * bullet_tail), by - (bvy * bullet_tail), 0);
            }
            bullet = (bx > X_MIN) && (bx < X_MAX) && (by > Y_MIN) && (by < Y_MAX);
            ++bullet_c;
        }

        Graphics::line(last[0][0], last[0][1], last[0][2], last[0][3], 0);

        if (bullet) {
            bx = bx + bvx;
            by = by + bvy;
            if (((bullet_c % bullet_cmax) == 0)) {
                Graphics::line(bx, by, bx - (bvx * bullet_tail), by - (bvy * bullet_tail), 2);
            }
        }

        vx += angleX;
        vy += angleY;

        vx *= friction;
        vy *= friction;

        x += vx;
        y += vy;

        if (x <= X_MIN) {
            x = X_MIN + 0.01f;
            bounceX = true;
        }

        if (x >= X_MAX) {
            x = X_MAX - 0.01f;
            bounceX = true;
        }

        if (y <= Y_MIN) {
            y = Y_MIN + 0.01f;
            bounceY = true;
        }

        if (y >= Y_MAX) {
            y = Y_MAX - 0.01f;
            bounceY = true;
        }

        last[0][0] = x;
        last[0][1] = y;
        last[0][2] = x - vx * tail;
        last[0][3] = y - vy * tail;

        Graphics::line(last[0][0], last[0][1], last[0][2], last[0][3], 1);

        if (bounceX || bounceY) {
            float angle = std::atan2(vy, vx);

            float left = angle - spark_deflect_rad;
            float right = angle + spark_deflect_rad;

            float ldx = cos(left);
            float ldy = sin(left);

            float rdx = cos(right);
            float rdy = sin(right);

            // last[0][0] = x - vx * 0.01f;
            // last[0][1] = y - vy * 0.01f;

            last[1][0] = x + ldx * spark_dist;
            last[1][1] = y + ldy * spark_dist;
            last[1][2] = last[1][0] + ldx * spark_tail;
            last[1][3] = last[1][1] + ldy * spark_tail;

            last[2][0] = x + rdx * spark_dist;
            last[2][1] = y + rdy * spark_dist;
            last[2][2] = last[2][0] + rdx * spark_tail;
            last[2][3] = last[2][1] + rdy * spark_tail;

            if (bounceX) {
                vx *= bounce;
            }

            if (bounceY) {
                vy *= bounce;
            }

            Graphics::line(last[1][0], last[1][1], last[1][2], last[1][3], 3);
            Graphics::line(last[2][0], last[2][1], last[2][2], last[2][3], 3);
        }

        if (joysticks.A1 && !bullet) {
            bullet = true;
            bx = x;
            by = y;

            bvx = vx * bullet_speed;
            bvy = vy * bullet_speed;
            bullet_cmax = 16 - std::sqrt(bvx * bvx + bvy * bvy) / 2;
            if (bullet_cmax > 16 || bullet_cmax == 0) {
                bullet_cmax = 1;
            }
        }

        // Graphics::text(10, 10, pos_str, 0);
        // snprintf(pos_str, 100, "X:%f, Y:%f", x, y);
        // Graphics::text(10, 10, pos_str, 1);

        // Graphics::text(10, 10, pos_str, 0);

        // angleX += 0.5;
        // angleY -= 0.5;

        // cube.draw(angleX, angleY);
        // snprintf(pos_str, 13, "X:%i, Y:%i", joysticks.X1, joysticks.Y1);
        // Graphics::text(10, 10, pos_str, 1);

        // if (++count >= 2) {
        //     Graphics::clear();
        //     count = 0;
        // }
    }

    getch();
    Graphics::setVideoMode(0x03); // text mode
    return 0;
}
