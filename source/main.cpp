
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <dos/graphics.hpp>
#include <dos/sound.hpp>
#include <graph.h>
#include <i86.h>
#include <process.h>
#include <stdio.h>

#include "debug.hpp"
#include "dummy.hpp"
#include "player.hpp"
#include "sprite_map.hpp"
#include "world.hpp"

static uint8_t total_bullets = 0;
static uint16_t sound_down = 0;
static uint16_t sound_blap = 0;
static uint16_t sound_crrsh = 0;
static uint16_t sound_freq = 0;
static bool silent = false;

void temp_handle_sound(Player &playerA, Player &playerB) {
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
        // sound(sound_freq);
    }
}

void temp_play_start_tune() {
    DOS::Sound::play(775, 10, 650, 10, 200, 3, 2, 8, 200);
    delay(100);
    DOS::Sound::play(775, 10, 650, 0, 200, 3, 2, 0, 200);
    delay(50);
    DOS::Sound::play(500, 9, 600, 10, 200, 3, 2, 5, 100);
    delay(100);
    DOS::Sound::play(0, 0, 0, 0, 0, 0, 0, 0, 0);
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

void temp_set_sprite(Player::status &status, size_t sprite_id) {
    uint8_t color = 0;

    switch (status.get()) {
        case Player::P_SS_OFF:
            color = 0;
            break;
        case Player::P_SS_GOOD:
            color = 1;
            break;
        case Player::P_SS_FAIL:
            color = 2;
            break;
        case Player::P_SS_FAIR:
            color = 3;
            break;
        default:
            return;
    }

    status.acknowledge();

    DOS::CGA::load_sprite(sprite_id, color, DOS::CGA::PERFECT, 0);
    debug::serial_print("s");
}

inline void temp_player_sprite_handle_a(Player &player) {
    temp_set_sprite(player.situation.ship.pilot, sprite::a_ship_pilot);
    temp_set_sprite(player.situation.ship.body, sprite::a_ship_body);
    temp_set_sprite(player.situation.ship.thruster.left, sprite::a_ship_thruster_left);
    temp_set_sprite(player.situation.ship.thruster.right, sprite::a_ship_thruster_right);

    temp_set_sprite(player.situation.bullet[0].indicator, sprite::a_bullet_indicator_0);
    temp_set_sprite(player.situation.bullet[0].head, sprite::a_bullet_head_0);
    temp_set_sprite(player.situation.bullet[0].body, sprite::a_bullet_body_0);
    temp_set_sprite(player.situation.bullet[0].boost, sprite::a_bullet_boost_0);

    temp_set_sprite(player.situation.bullet[1].indicator, sprite::a_bullet_indicator_1);
    temp_set_sprite(player.situation.bullet[1].head, sprite::a_bullet_head_1);
    temp_set_sprite(player.situation.bullet[1].body, sprite::a_bullet_body_1);
    temp_set_sprite(player.situation.bullet[1].boost, sprite::a_bullet_boost_1);

    temp_set_sprite(player.situation.panel.gun_ready, sprite::a_p_gun_ready);
    temp_set_sprite(player.situation.panel.left_authority, sprite::a_p_authority_left);
    temp_set_sprite(player.situation.panel.low_speed, sprite::a_p_low_speed);
    temp_set_sprite(player.situation.panel.right_authority, sprite::a_p_authority_right);
    temp_set_sprite(player.situation.panel.ammo_low, sprite::a_p_ammo_low);
    temp_set_sprite(player.situation.panel.auto_pilot, sprite::a_p_auto_pilot);
    temp_set_sprite(player.situation.panel.fuel_low, sprite::a_p_fuel_low);
    temp_set_sprite(player.situation.panel.fire, sprite::a_p_fire);
    temp_set_sprite(player.situation.panel.booster_damage, sprite::a_p_booster_damage);
    temp_set_sprite(player.situation.panel.hull_breach, sprite::a_p_breach);
    temp_set_sprite(player.situation.panel.nuclear_meltdown, sprite::a_p_nuclear_melt);
    temp_set_sprite(player.situation.panel.dead, sprite::a_p_dead);
}

inline void temp_player_sprite_handle_b(Player &player) {
    temp_set_sprite(player.situation.ship.pilot, sprite::b_ship_pilot);
    temp_set_sprite(player.situation.ship.body, sprite::b_ship_body);
    temp_set_sprite(player.situation.ship.thruster.left, sprite::b_ship_thruster_left);
    temp_set_sprite(player.situation.ship.thruster.right, sprite::b_ship_thruster_right);

    temp_set_sprite(player.situation.bullet[0].indicator, sprite::b_bullet_indicator_0);
    temp_set_sprite(player.situation.bullet[0].head, sprite::b_bullet_head_0);
    temp_set_sprite(player.situation.bullet[0].body, sprite::b_bullet_body_0);
    temp_set_sprite(player.situation.bullet[0].boost, sprite::b_bullet_boost_0);

    temp_set_sprite(player.situation.bullet[1].indicator, sprite::b_bullet_indicator_1);
    temp_set_sprite(player.situation.bullet[1].head, sprite::b_bullet_head_1);
    temp_set_sprite(player.situation.bullet[1].body, sprite::b_bullet_body_1);
    temp_set_sprite(player.situation.bullet[1].boost, sprite::b_bullet_boost_1);

    temp_set_sprite(player.situation.panel.gun_ready, sprite::b_p_gun_ready);
    temp_set_sprite(player.situation.panel.left_authority, sprite::b_p_authority_left);
    temp_set_sprite(player.situation.panel.low_speed, sprite::b_p_low_speed);
    temp_set_sprite(player.situation.panel.right_authority, sprite::b_p_authority_right);
    temp_set_sprite(player.situation.panel.ammo_low, sprite::b_p_ammo_low);
    temp_set_sprite(player.situation.panel.auto_pilot, sprite::b_p_auto_pilot);
    temp_set_sprite(player.situation.panel.fuel_low, sprite::b_p_fuel_low);
    temp_set_sprite(player.situation.panel.fire, sprite::b_p_fire);
    temp_set_sprite(player.situation.panel.booster_damage, sprite::b_p_booster_damage);
    temp_set_sprite(player.situation.panel.hull_breach, sprite::b_p_breach);
    temp_set_sprite(player.situation.panel.nuclear_meltdown, sprite::b_p_nuclear_melt);
    temp_set_sprite(player.situation.panel.dead, sprite::b_p_dead);
}

inline void temp_set_sprites(Player &playerA, Player &playerB) {
    static const size_t max = 100;
    static size_t i = 0;
    if ((i++ % max == 0) && player_situation_update) {
        temp_player_sprite_handle_a(playerA);
        temp_player_sprite_handle_b(playerB);
        player_situation_update = true;
    }
}

void clear_screen() {
    DOS::Draw::rectangle(world::X_MIN - 4, world::Y_MIN - 4, world::X_MAX + 4, world::Y_MAX + 4, 0, true);
}

static Player::situation_t sit_mem_A;
static Player::situation_t sit_mem_B;

int main() {
    debug::serial_init();
    debug::serial_printf("start");

    DOS::Video::initialize();
    DOS::Sound::initialize();
#ifdef DOSBOX
    DOS::Input::Joystick::initialize(true);
#else
    DOS::Input::Joystick::initialize(false);
#endif

    Player playerA(DOS::Input::Joystick::playerA, sit_mem_A);

    dummy::Dummy inputB;
    Player playerB(inputB, sit_mem_B);
    inputB.set_target(&playerA);
    inputB.set_player(&playerB);
#ifdef DOSBOX
    dummy::Dummy inputx[6];
    Player::situation_t sit_mem_C;
    Player player0(inputx[0], sit_mem_C);
    inputx[0].set_player(&player0);
    Player player1(inputx[1], sit_mem_C);
    inputx[1].set_player(&player1);
    Player player2(inputx[2], sit_mem_C);
    inputx[2].set_player(&player2);
    Player player3(inputx[3], sit_mem_C);
    inputx[3].set_player(&player3);
    Player player4(inputx[4], sit_mem_C);
    inputx[4].set_player(&player4);
    Player player5(inputx[5], sit_mem_C);
    inputx[5].set_player(&player5);
    // Player player6(inputx[6], sit_mem_C);
    // inputx[6].set_player(&player6);
    // Player player7(inputx[7], sit_mem_C);
    // inputx[7].set_player(&player7);

    inputx[0].set_target(&player2);
    inputx[1].set_target(&playerA);
    inputx[2].set_target(&player0);
    inputx[3].set_target(&playerA);
    inputx[4].set_target(&player1);
    inputx[5].set_target(&playerA);
    // inputx[6].set_target(&player3);
    // inputx[7].set_target(&player4);
#endif

    FILE *f
        = fopen("joystick.cal", "rb");
    if (f) {
        uint8_t *dataA = (uint8_t *)&DOS::Input::Joystick::playerA.cal;
        uint8_t *dataB = (uint8_t *)&DOS::Input::Joystick::playerB.cal;
        const size_t data_size = sizeof(DOS::Input::Joystick::playerA.cal);
        fread(dataA, 1, data_size, f);
        fread(dataB, 1, data_size, f);
        fclose(f);
        DOS::Draw::text(10, 10, "Joystick Cal Loaded", 3);
        DOS::Draw::text(10, 10, "Joystick Cal Loaded", 0);
    }

    DOS::CGA::display_cga("frame.cga", DOS::CGA::SEMI);
    DOS::CGA::display_cga("title.cga", DOS::CGA::SEMI);
    DOS::CGA::load_sprites("sprites.bin");

    DOS::Sound::silence();
    temp_play_start_tune();

    bool run = true;
    while (run) {
        if (kbhit()) {
            int ch = getch();
            switch (ch) {
                case 'x':
                    run = false;
                    break;
                case 'y': {
                    const uint8_t *dataA = (uint8_t *)&DOS::Input::Joystick::playerA.cal;
                    const uint8_t *dataB = (uint8_t *)&DOS::Input::Joystick::playerB.cal;
                    const size_t data_size = sizeof(DOS::Input::Joystick::playerA.cal);

                    FILE *f = fopen("joystick.cal", "wb");
                    if (!f) {
                        debug::serial_printf("Could not open joystick.cal!\n");
                        break;
                    }

                    size_t written = fwrite(dataA, 1, data_size, f);
                    written += fwrite(dataB, 1, data_size, f);
                    if (written != (data_size * 2)) {
                        debug::serial_printf("Write failed!\n");
                        fclose(f);
                        break;
                    }

                    fclose(f);

                } break;
                case 'r':
                    sound_down = 0;
                    sound_blap = 0;
                    sound_crrsh = 0;
                    sound_freq = 0;
                    silent = 1;
                    DOS::Sound::silence();
                    break;
                default:
                    break;
            }
        }

        static const size_t max = 4;
        static size_t i = 0;
        if (i++ % max == 0) {
            inputB.update();
        }
        DOS::Input::Joystick::update();

#ifdef DOSBOX
        if (playerB.valid())
            for (size_t i = 0; i < (sizeof(inputx) / sizeof(inputx[0])); i++) {
                inputx[i].update();
            }

#endif

        if (!playerA.valid() && (DOS::Input::Joystick::playerA.fire || DOS::Input::Joystick::playerA.alt)) {
            temp_play_A_join();
            clear_screen();
            world::add_player(playerA);
        }

        if (!playerB.valid() && (DOS::Input::Joystick::playerB.fire || DOS::Input::Joystick::playerB.alt)) {
            temp_play_B_join();
            clear_screen();
            world::add_player(playerB);
#ifdef DOSBOX
            world::add_player(player0);
            world::add_player(player1);
            world::add_player(player2);
            world::add_player(player3);
            world::add_player(player4);
            world::add_player(player5);
            // world::add_player(player6);
            // world::add_player(player7);
#endif
        }

        for (size_t i = 0; i < world::current_players; i++) {
            world::players[i]->step();
        }

        temp_handle_sound(playerA, playerB);
        temp_set_sprites(playerA, playerB);

        world::draw();

        if (world::current_players == 0) {
            DOS::Input::Joystick::playerA.calibrate_step();
            DOS::Input::Joystick::playerB.calibrate_step();
        }
    }

    DOS::Sound::silence();
    return 0;
}
