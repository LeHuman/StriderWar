
#include <iostream>

#include <conio.h>
#include <dos.h>
#include <dos/graphics.hpp>
#include <dos/sound.hpp>
#include <graph.h>
#include <i86.h>
#include <math.h>
#include <process.h>
#include <song_data.hpp>
#include <stdio.h>
#include <time.hpp>

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

volatile size_t tick = 0;
const size_t temp_sound_hertz_x10 = 31;
volatile int note = 0;
volatile int vol = 0;

struct PSGChannel {
    const uint8_t *data;
    const uint16_t *rle;

    const size_t command_len;
    const size_t length;
    const size_t rle_length;
    const bool has_rle;

    struct Wait {
        uint16_t cycles;
        uint16_t index;
        Wait(uint16_t cycles, uint16_t index) : cycles(cycles), index(index) {}
    } wait;

    uint16_t current_index;
    uint16_t rle_index;

    PSGChannel(
        const uint8_t *data,
        const uint16_t *rle,
        const size_t &length,
        const size_t &rle_length, const bool is_noise) : data(data), rle(rle), command_len(is_noise ? 2 : 3), length(length), rle_length(rle_length), has_rle(rle_length > 0), wait(0, 0), current_index(0), rle_index(0) {
        reset();
    }

    void reset() {
        current_index = 0;
        rle_index = 0;

        if (has_rle) {
            wait.cycles = rle[rle_index] & 0xF;
            wait.index = rle[rle_index] >> 4;
        }
    }

    void step() {
        if ((current_index == wait.index)) {
            if (wait.cycles > 0) {
                --wait.cycles;
                return;
            } else if (has_rle) {
                ++rle_index;
                rle_index %= rle_length;
                wait.cycles = rle[rle_index] & 0xF;
                wait.index = rle[rle_index] >> 4;
            }
        }

        for (size_t i = 0; i < command_len; i++) {
            outp(0xC0, data[current_index + i]);
        }

        current_index += command_len;
        current_index %= length;

        if (current_index == 0) {
            reset();
        }
    }
};

PSGChannel pattern0(SongData::Pattern0::data, SongData::Pattern0::rle, SongData::Pattern0::LEN, SongData::Pattern0::RLE_LEN, false);
PSGChannel pattern1(SongData::Pattern1::data, SongData::Pattern1::rle, SongData::Pattern1::LEN, SongData::Pattern1::RLE_LEN, false);
PSGChannel pattern2(SongData::Pattern2::data, SongData::Pattern2::rle, SongData::Pattern2::LEN, SongData::Pattern2::RLE_LEN, false);
PSGChannel drum0(SongData::Drum0::data, SongData::Drum0::rle, SongData::Drum0::LEN, SongData::Drum0::RLE_LEN, true);
PSGChannel drum1(SongData::Drum1::data, SongData::Drum1::rle, SongData::Drum1::LEN, SongData::Drum1::RLE_LEN, true);
PSGChannel drum2(SongData::Drum2::data, SongData::Drum2::rle, SongData::Drum2::LEN, SongData::Drum2::RLE_LEN, true);

bool swap = true;
size_t note_count = 0;
size_t swap_count = 0;

void interrupt temp_sound() {
    if ((tick % 5) == 0) {
        drum0.step();
        drum1.step();
        drum2.step();

        if (swap) {
            pattern0.step();
        } else {
            pattern1.step();
        }

        pattern2.step();

        ++note_count;

        if ((note_count % 32) == 0) {
            swap = !swap;
            ++swap_count;
            pattern0.reset();
            pattern1.reset();
        }

        if ((note_count % 64) == 0) {
            pattern2.reset();
        }
    }

    ++tick;
    outp(0x20, 0x20);
}

inline void temp_handle_sound(Player &playerA, Player &playerB) {
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
}

inline void temp_player_sprite_handle_a(Player &player) {
    temp_set_sprite(player.situation->ship.pilot, sprite::a_ship_pilot);
    temp_set_sprite(player.situation->ship.body, sprite::a_ship_body);
    temp_set_sprite(player.situation->ship.thruster.left, sprite::a_ship_thruster_left);
    temp_set_sprite(player.situation->ship.thruster.right, sprite::a_ship_thruster_right);

    temp_set_sprite(player.situation->bullet[0].indicator, sprite::a_bullet_indicator_0);
    temp_set_sprite(player.situation->bullet[0].head, sprite::a_bullet_head_0);
    temp_set_sprite(player.situation->bullet[0].body, sprite::a_bullet_body_0);
    temp_set_sprite(player.situation->bullet[0].boost, sprite::a_bullet_boost_0);

    temp_set_sprite(player.situation->bullet[1].indicator, sprite::a_bullet_indicator_1);
    temp_set_sprite(player.situation->bullet[1].head, sprite::a_bullet_head_1);
    temp_set_sprite(player.situation->bullet[1].body, sprite::a_bullet_body_1);
    temp_set_sprite(player.situation->bullet[1].boost, sprite::a_bullet_boost_1);

    temp_set_sprite(player.situation->panel.gun_ready, sprite::a_p_gun_ready);
    temp_set_sprite(player.situation->panel.left_authority, sprite::a_p_authority_left);
    temp_set_sprite(player.situation->panel.low_speed, sprite::a_p_low_speed);
    temp_set_sprite(player.situation->panel.right_authority, sprite::a_p_authority_right);
    temp_set_sprite(player.situation->panel.ammo_low, sprite::a_p_ammo_low);
    temp_set_sprite(player.situation->panel.auto_pilot, sprite::a_p_auto_pilot);
    temp_set_sprite(player.situation->panel.fuel_low, sprite::a_p_fuel_low);
    temp_set_sprite(player.situation->panel.fire, sprite::a_p_fire);
    temp_set_sprite(player.situation->panel.booster_damage, sprite::a_p_booster_damage);
    temp_set_sprite(player.situation->panel.hull_breach, sprite::a_p_breach);
    temp_set_sprite(player.situation->panel.nuclear_meltdown, sprite::a_p_nuclear_melt);
    temp_set_sprite(player.situation->panel.dead, sprite::a_p_dead);
}

inline void temp_player_sprite_handle_b(Player &player) {
    temp_set_sprite(player.situation->ship.pilot, sprite::b_ship_pilot);
    temp_set_sprite(player.situation->ship.body, sprite::b_ship_body);
    temp_set_sprite(player.situation->ship.thruster.left, sprite::b_ship_thruster_left);
    temp_set_sprite(player.situation->ship.thruster.right, sprite::b_ship_thruster_right);

    temp_set_sprite(player.situation->bullet[0].indicator, sprite::b_bullet_indicator_0);
    temp_set_sprite(player.situation->bullet[0].head, sprite::b_bullet_head_0);
    temp_set_sprite(player.situation->bullet[0].body, sprite::b_bullet_body_0);
    temp_set_sprite(player.situation->bullet[0].boost, sprite::b_bullet_boost_0);

    temp_set_sprite(player.situation->bullet[1].indicator, sprite::b_bullet_indicator_1);
    temp_set_sprite(player.situation->bullet[1].head, sprite::b_bullet_head_1);
    temp_set_sprite(player.situation->bullet[1].body, sprite::b_bullet_body_1);
    temp_set_sprite(player.situation->bullet[1].boost, sprite::b_bullet_boost_1);

    temp_set_sprite(player.situation->panel.gun_ready, sprite::b_p_gun_ready);
    temp_set_sprite(player.situation->panel.left_authority, sprite::b_p_authority_left);
    temp_set_sprite(player.situation->panel.low_speed, sprite::b_p_low_speed);
    temp_set_sprite(player.situation->panel.right_authority, sprite::b_p_authority_right);
    temp_set_sprite(player.situation->panel.ammo_low, sprite::b_p_ammo_low);
    temp_set_sprite(player.situation->panel.auto_pilot, sprite::b_p_auto_pilot);
    temp_set_sprite(player.situation->panel.fuel_low, sprite::b_p_fuel_low);
    temp_set_sprite(player.situation->panel.fire, sprite::b_p_fire);
    temp_set_sprite(player.situation->panel.booster_damage, sprite::b_p_booster_damage);
    temp_set_sprite(player.situation->panel.hull_breach, sprite::b_p_breach);
    temp_set_sprite(player.situation->panel.nuclear_meltdown, sprite::b_p_nuclear_melt);
    temp_set_sprite(player.situation->panel.dead, sprite::b_p_dead);
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

    if (DOS::Input::Joystick::load_calibration("joystick.cal")) {
        DOS::Draw::text(10, 10, "Joystick Cal Loaded", 3);
        DOS::Draw::text(10, 10, "Joystick Cal Loaded", 0);
    }

    DOS::Input::Keyboard::initialize();

    dummy::Dummy autopilot;
    Player playerA(nullptr, &sit_mem_A);
    Player playerB(nullptr, &sit_mem_B);

    DOS::CGA::display_cga("frame.cga", DOS::CGA::SEMI);
    DOS::CGA::display_cga("title.cga", DOS::CGA::SEMI);
    DOS::CGA::load_sprites("sprites.bin");

    DOS::Sound::silence();

    bool enable_joystick = false;
    bool enable_keyboard = false;

    Time::ISR::initialize(temp_sound, temp_sound_hertz_x10);

    while (true) {
        if (DOS::Input::Keyboard::exit_requested) {
            break;
        }

        if (DOS::Input::Keyboard::save_requested) {
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
        }

        bool a_valid = playerA.valid();
        bool b_valid = playerB.valid();

        if (DOS::Input::Keyboard::bot_requested) {
            if (a_valid && !b_valid) {
                playerB.set_input(&autopilot);
                autopilot.set_player(&playerB);
                autopilot.set_target(&playerA);
                if (world::add_player(playerB) > -1) {
                    b_valid = true;
                }
            } else if (b_valid && !a_valid) {
                playerA.set_input(&autopilot);
                autopilot.set_player(&playerA);
                autopilot.set_target(&playerB);
                if (world::add_player(playerA) > -1) {
                    a_valid = true;
                }
            }
        }

        if (DOS::Input::Keyboard::reset_requested) {
            bool enable_joystick = false;
            bool enable_keyboard = false;
            playerA.set_input(nullptr);
            playerB.set_input(nullptr);
            autopilot.set_player(nullptr);
            autopilot.set_target(nullptr);
            world::clear_players();
            clear_screen();
        }

        static const size_t max = 4;
        static size_t i = 0;
        if (i++ % max == 0) {
            autopilot.update();
        }

        if (enable_joystick || !enable_keyboard) {
            DOS::Input::Joystick::update();
        }

        if (enable_keyboard) {
            DOS::Input::Keyboard::update();
        }

        if (!a_valid) {
            if (DOS::Input::Joystick::playerA.fire) {
                playerA.set_input(&DOS::Input::Joystick::playerA);
                enable_joystick = true;
            } else if (DOS::Input::Keyboard::playerA.fire) {
                playerA.set_input(&DOS::Input::Keyboard::playerA);
                enable_keyboard = true;
            }

            if (playerA.input != nullptr) {
                clear_screen();
                world::add_player(playerA);
            }
        }

        if (!b_valid) {
            if (DOS::Input::Joystick::playerB.fire) {
                playerB.set_input(&DOS::Input::Joystick::playerB);
                enable_joystick = true;
            } else if (DOS::Input::Keyboard::playerB.fire) {
                playerB.set_input(&DOS::Input::Keyboard::playerB);
                enable_keyboard = true;
            }

            if (playerB.input != nullptr) {
                clear_screen();
                world::add_player(playerB);
            }
        }

        // FIXME: Not working on hardware?
        temp_handle_sound(playerA, playerB);
        temp_set_sprites(playerA, playerB);

        world::step();
        world::draw();

        if (world::current_players == 0) {
            DOS::Input::Joystick::playerA.calibrate_step();
            DOS::Input::Joystick::playerB.calibrate_step();
        }
    }

    Time::ISR::shutdown();
    DOS::Input::Keyboard::shutdown();
    DOS::Sound::silence();
    return 0;
}
