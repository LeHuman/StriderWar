#include <cmath>
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cube.hpp"
#include "fixed.hpp"
#include "fixed_math.hpp"
#include "graphics.hpp"
#include "i86.h"
#include "joystick.hpp"
#include "sound.hpp"

static const Fixed X_MIN = 0.0f;
static const Fixed X_MAX = 640.0f / 2.0f;
static const Fixed Y_MIN = 0.0f;
static const Fixed Y_MAX = 200.0f;

int main() {
    FixedMath::init();
    Graphics::init();

    Joysticks joysticks(0x201);

    int count = 0;

    const Fixed gravity = 0.185f;
    const Fixed friction = 0.98f;
    const Fixed bounce = -0.86f;
    const Fixed tail = 2.0f;

    const Fixed bullet_speed = 2.0f;
    const Fixed bullet_tail = 0.5f;

    const Fixed spark_dist = 5.0f;
    const Fixed spark_tail = -10.0f;
    const Fixed spark_deflect_rad = 0.5235988;

    int last[3][4] = {0};

    Fixed x = X_MAX / 2.0f;
    Fixed y = Y_MIN / 2.0f;
    Fixed vx = 20.0f;
    Fixed vy = 10.0f;

    Fixed bx = 0.0f;
    Fixed by = 0.0f;
    Fixed bvx = 0.0f;
    Fixed bvy = 0.0f;

    unsigned int bullet_c = 0;
    unsigned int bullet_cmax = 0;

    bool bullet = false;
    bool bounceX = false;
    bool bounceY = false;

    while (!kbhit()) {
        joysticks.update();

        vy += gravity;

        Fixed angleX = (Fixed)(joysticks.X1 - 3) / 4;
        Fixed angleY = (Fixed)(joysticks.Y1 - 3) / 4;

        if (bounceX || bounceY) {
            Graphics::line(last[1][0], last[1][1], last[1][2], last[1][3], 0);
            Graphics::line(last[2][0], last[2][1], last[2][2], last[2][3], 0);
            bounceX = false;
            bounceY = false;
        }

        if (bullet) {
            if (((bullet_c % bullet_cmax) == 0)) {
                Graphics::line(bx.toInt(), by.toInt(), (bx - (bvx * bullet_tail)).toInt(),( by - (bvy * bullet_tail)).toInt(), 0);
            }
            bullet = (bx > X_MIN) && (bx < X_MAX) && (by > Y_MIN) && (by < Y_MAX);
            ++bullet_c;
        }

        Graphics::line(last[0][0], last[0][1], last[0][2], last[0][3], 0);

        if (bullet) {
            bx = bx + bvx;
            by = by + bvy;
            if (((bullet_c % bullet_cmax) == 0)) {
                Graphics::line(bx.toInt(), by.toInt(), (bx - (bvx * bullet_tail)).toInt(),( by - (bvy * bullet_tail)).toInt(), 2);
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
            Fixed angle = FixedMath::atan2(vy, vx);

            Fixed left = angle - spark_deflect_rad;
            Fixed right = angle + spark_deflect_rad;

            Fixed ldx = FixedMath::cos(left);
            Fixed ldy = FixedMath::sin(left);

            Fixed rdx = FixedMath::cos(right);
            Fixed rdy = FixedMath::sin(right);

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
            bullet_cmax = 16 - FixedMath::sqrt(bvx * bvx + bvy * bvy) / 2;
            if (bullet_cmax > 16 || bullet_cmax == 0) {
                bullet_cmax = 1;
            }
        }
    }

    getch();
    return 0;
}
