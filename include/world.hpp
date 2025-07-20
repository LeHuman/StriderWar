#pragma once

#include <dos/graphics.hpp>
#include <math.hpp>

#include "player.hpp"

namespace world {

static const math::Fixed gravity = 0.45f;
static const math::Fixed friction = 0.999f;
static const math::Fixed bounce_mult = -0.92f;
static const math::Fixed tail_mult = 2.0f;

static const math::Fixed bullet_speed = 0.3f;
static const math::Fixed bullet_tail = 2.0f;

// (200-8) / 200=0.96
// (320-68) / 320=0.7875
// 1-0.7875=0.2125
static const uint16_t X_MIN = DOS::Video::WIDTH * 0.2125f;
static const uint16_t X_MAX = DOS::Video::WIDTH * 0.7875f;
static const uint16_t Y_MIN = DOS::Video::HEIGHT * 0.04f;
static const uint16_t Y_MAX = DOS::Video::HEIGHT * 0.96f;

static const uint16_t X_CENTER = DOS::Video::WIDTH / 2;
static const uint16_t Y_CENTER = DOS::Video::HEIGHT / 2;

static const size_t MAX_PLAYERS = 2;
static const size_t MAX_EXPLOSIONS = 8;

extern size_t current_players;
extern Player *players[MAX_PLAYERS];

int add_player(Player &player);
void explode(Bullet &bullet);

} // namespace world
