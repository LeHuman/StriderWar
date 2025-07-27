#pragma once

#include <dos/graphics.hpp>
#include <math.hpp>

#include "player.hpp"

namespace world {

static const math::Fixed gravity = 0.45f;
static const math::Fixed friction = 0.999f;
static const math::Fixed bounce_mult = -0.92f;
static const math::Fixed tail_mult = 2.5f;

static const math::Fixed bullet_speed = 0.3f;
static const math::Fixed bullet_tail = 3.5f;

// (200-8) / 200=0.96
// (320-68) / 320=0.7875
// 1-0.7875=0.2125
static const uint16_t X_MIN = DOS::Video::WIDTH * 0.2125f;
static const uint16_t X_MAX = DOS::Video::WIDTH * 0.7875f;
static const uint16_t Y_MIN = DOS::Video::HEIGHT * 0.04f;
static const uint16_t Y_MAX = DOS::Video::HEIGHT * 0.96f;

static const uint16_t X_CENTER = DOS::Video::WIDTH / 2;
static const uint16_t Y_CENTER = DOS::Video::HEIGHT / 2;

#ifdef WORLD_OVERRIDE
static const size_t MAX_PLAYERS = 10;
#else
static const size_t MAX_PLAYERS = 2;
#endif

static const size_t MAX_EXPLOSIONS = 8;

extern size_t current_players;
extern Player *players[MAX_PLAYERS];

inline void clear_players() {
    for (size_t i = 0; i < world::current_players; i++) {
        players[i]->id = -1;
    }
    current_players = 0;
}

int add_player(Player &player);

inline bool check_bullet(const Bullet &bullet, const Player *player) {
    const int blast_radius = 10 + bullet.mult.damage;
    const math::Fixed &x = bullet.entity.x;
    const math::Fixed &y = bullet.entity.y;

    const math::Fixed &sx = player->ship.entity.x;
    const math::Fixed &sy = player->ship.entity.y;

    const int dx = (int)(sx - x) / 2;
    const int dy = (int)(sy - y) / 2;

    return ((dx * dx) + (dy * dy)) < (blast_radius * blast_radius / 2);
}

inline void step() {
    for (size_t i = 0; i < current_players; i++) {
        players[i]->step();
    }
}

inline void draw() {
    for (size_t i = 0; i < current_players; i++) {
        players[i]->draw();
    }
}

void explode(Bullet &bullet);

} // namespace world
