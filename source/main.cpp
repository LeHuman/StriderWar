
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <dos/graphics.hpp>
#include <dos/sound.hpp>
#include <dos/time.hpp>
#include <graph.h>
#include <i86.h>
#include <stdio.h>

#include "debug.hpp"
#include "player.hpp"

int main() {
    DOS::Video::initialize();
    DOS::Sound::initialize();
    DOS::Input::Joystick::initialize();

    Player playerA(DOS::Input::Joystick::playerA);
    Player playerB(DOS::Input::Joystick::playerB);

    uint8_t total_bullets = 0;
    uint16_t sound_down = 0;
    uint16_t sound_blap = 0;
    uint16_t sound_freq = 0;
    bool silent = 0;

    bool player_a_on = false;
    bool player_b_on = false;

    DOS::CGA::display_cga("frame.cga", DOS::CGA::SEMI);
    DOS::CGA::display_cga("title.cga", DOS::CGA::SEMI);
    DOS::CGA::load_sprites("sprites.bin");

    DOS::Sound::play(775, 10, 650, 10, 200, 3, 2, 8, 200);
    delay(100);
    DOS::Sound::play(775, 10, 650, 0, 200, 3, 2, 0, 200);
    delay(50);
    DOS::Sound::play(500, 9, 600, 10, 200, 3, 2, 5, 100);
    delay(100);
    DOS::Sound::silence();

    size_t sprite_i = 0;
    size_t sprite_c = 0;

    bool run = true;

    // DOS::Time::Timer timer;
    // const uint32_t frameDuration = 1000; // target ~30 FPS

    while (run) {
        // timer.beginFrame();

        if (kbhit()) {
            run = getch() != 'x';
        }

        DOS::Input::Joystick::update();

        // DOS::CGA::load_sprite(sprite_i, sprite_c, DOS::CGA::PERFECT, 0);
        sprite_i = abs(rand()) % *DOS::CGA::sprite_bank.length;
        sprite_c = ((sprite_c + 1) % 3) + 1;

        if (player_a_on) {
            playerA.step();
        } else if (DOS::Input::Joystick::playerA.fire || DOS::Input::Joystick::playerA.alt) {
            player_a_on = true;

            sound(784);
            delay(30);

            nosound();
            delay(70);

            sound(784);
            delay(60);

            nosound();
            delay(100);

            sound(784);
            delay(100);

            nosound();
            delay(100);

            sound(698);
            delay(70);

            nosound();
            delay(180);

            sound(784);
            delay(100);
            nosound();
        } else if (!player_b_on) {
            DOS::Input::Joystick::playerA.calibrate_step();
        }

        if (player_b_on) {
            playerB.step();
        } else if (DOS::Input::Joystick::playerB.fire || DOS::Input::Joystick::playerB.alt) {
            player_b_on = true;

            sound(1047);
            delay(30);

            nosound();
            delay(70);

            sound(1047);
            delay(60);

            nosound();
            delay(100);

            sound(784);
            delay(100);

            nosound();
            delay(100);

            sound(698);
            delay(70);

            nosound();
            delay(180);

            sound(784);
            delay(100);
            nosound();
        } else if (!player_a_on) {
            DOS::Input::Joystick::playerB.calibrate_step();
        }

        if ((playerA.enabled_bullets + playerB.enabled_bullets) != total_bullets) {
            if ((playerA.enabled_bullets + playerB.enabled_bullets) > total_bullets) {
                sound_down = 800;
                silent = false;
            } else if ((playerA.enabled_bullets + playerB.enabled_bullets) == 0) {
                sound_down /= 2;
            }
            total_bullets = playerA.enabled_bullets + playerB.enabled_bullets;
        }

        if (!sound_blap && (playerA.bounced || playerA.bounced || playerB.bounced || playerB.bounced)) {
            sound_blap = 150;
            silent = false;
        }

        if (sound_blap) {
            sound_blap -= 50;
        }

        if (sound_down > 100) {
            sound_down -= 25;
        } else if (sound_down != 0) {
            sound_down = 0;
        }

        if (sound_blap) {
            sound_freq = sound_blap;
        } else if (sound_down) {
            sound_freq = sound_down;
        } else if (!silent) {
            nosound();
            silent = true;
            sound_freq = 0;
        }

        if (sound_freq) {
            sound(sound_freq);
        }

        if (player_a_on) {
            playerA.draw();
        }

        if (player_b_on) {
            playerB.draw();
        }

        // static uint16_t ty = 0;
        // uint16_t time = timer.elapsedMs();
        // uint8_t dist = time / 50;

        // DOS::Draw::line(dist, ty, dist * 2, ty, 0);
        // DOS::Draw::line(0, ty, dist, ty, sprite_c);

        // ty = (ty + 1) % DOS::Video::HEIGHT;
    }

    DOS::Sound::silence();
    return 0;
}
