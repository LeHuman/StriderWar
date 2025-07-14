
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <dos/graphics.hpp>
#include <dos/sound.hpp>
#include <dos/time.hpp>
#include <graph.h>
#include <i86.h>
#include <process.h>
#include <stdio.h>

#include "debug.hpp"
#include "dummy.hpp"
#include "player.hpp"
#include "world.hpp"

void temp_handle_sound(Player &playerA, Player &playerB) {
    static uint8_t total_bullets = 0;
    static uint16_t sound_down = 0;
    static uint16_t sound_blap = 0;
    static uint16_t sound_crrsh = 0;
    static uint16_t sound_freq = 0;
    static bool silent = false;

    if ((playerA.enabled_bullets + playerB.enabled_bullets) != total_bullets) {
        if ((playerA.enabled_bullets + playerB.enabled_bullets) > total_bullets) {
            sound_down = 800;
            silent = false;
        } else if ((playerA.enabled_bullets + playerB.enabled_bullets) == 0) {
            sound_down /= 2;
        }
        if ((playerA.enabled_bullets + playerB.enabled_bullets) < total_bullets) {
            sound_crrsh = 60;
            silent = false;
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

    if (sound_crrsh > 45) {
        sound_crrsh--;
    } else if (sound_crrsh != 0) {
        sound_crrsh = 0;
    }

    if (sound_crrsh) {
        sound_freq = sound_crrsh;
    } else if (sound_down) {
        sound_freq = sound_down;
    } else if (sound_blap) {
        sound_freq = sound_blap;
    } else if (!silent) {
        nosound();
        silent = true;
        sound_freq = 0;
    }

    if (sound_freq) {
        sound(sound_freq);
    }
}

void temp_play_start_tune() {
    DOS::Sound::play(775, 10, 650, 10, 200, 3, 2, 8, 200);
    delay(100);
    DOS::Sound::play(775, 10, 650, 0, 200, 3, 2, 0, 200);
    delay(50);
    DOS::Sound::play(500, 9, 600, 10, 200, 3, 2, 5, 100);
    delay(100);
    DOS::Sound::silence();
}

void temp_play_A_join() {
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
}

void temp_play_B_join() {
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
}

void clear_screen() {
    DOS::Draw::rectangle(world::X_MIN - 4, world::Y_MIN - 4, world::X_MAX + 4, world::Y_MAX + 4, 0, true);
}

int main() {
    debug::serial_init();
    debug::serial_printf("start");

    DOS::Video::initialize();
    DOS::Sound::initialize();
    DOS::Input::Joystick::initialize();

    Player playerA(DOS::Input::Joystick::playerA);

    dummy::Dummy inputB;
    Player playerB(inputB);
    inputB.set_player(&playerB);

    DOS::CGA::display_cga("frame.cga", DOS::CGA::SEMI);
    DOS::CGA::display_cga("title.cga", DOS::CGA::SEMI);
    DOS::CGA::load_sprites("sprites.bin");

    DOS::Sound::silence();
    temp_play_start_tune();

    bool run = true;
    while (run) {
        if (kbhit()) {
            run = getch() != 'x';
        }

        inputB.update();
        DOS::Input::Joystick::update();

        debug::serial_printf("x:%i y:%i x:%i y:%i\n", DOS::Input::Joystick::playerA.x, DOS::Input::Joystick::playerA.y, inputB.x, inputB.y);
        // debug::serial_printf("x:%i y:%i dx:%i dy:%i\n", (int)playerA.ship.x, (int)playerA.ship.y, (int)world::X_CENTER - (int)playerA.ship.x, (int)world::Y_CENTER - (int)playerA.ship.y);

        if (!playerA.valid() && (DOS::Input::Joystick::playerA.fire || DOS::Input::Joystick::playerA.alt)) {
            temp_play_A_join();
            clear_screen();
            world::add_player(playerA);
        }

        if (!playerB.valid() && (DOS::Input::Joystick::playerB.fire || DOS::Input::Joystick::playerB.alt)) {
            temp_play_B_join();
            clear_screen();
            world::add_player(playerB);
        }

        for (size_t i = 0; i < world::current_players; i++) {
            world::players[i]->step();
        }

        temp_handle_sound(playerA, playerB);

        for (size_t i = 0; i < world::current_players; i++) {
            world::players[i]->draw();
        }

        if (world::current_players == 0) {
            DOS::Input::Joystick::playerA.calibrate_step();
            DOS::Input::Joystick::playerB.calibrate_step();
        }
    }

    DOS::Sound::silence();
    return 0;
}
