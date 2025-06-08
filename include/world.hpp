#pragma once

#include <dos/math.hpp>
#include <dos/graphics.hpp>

static const Fixed gravity = 0.45f;
static const Fixed friction = 0.999f;
static const Fixed bounce_mult = -0.92f;
static const Fixed tail_mult = 2.0f;

static const Fixed bullet_speed = 2.0f;
static const Fixed bullet_tail = 2.0f;

// (200-8) / 200=0.96
// (320-68) / 320=0.7875
// 1-0.7875=0.2125

static const uint16_t X_MIN = DOS::Video::WIDTH * 0.2125f;
static const uint16_t X_MAX = DOS::Video::WIDTH * 0.7875f;
static const uint16_t Y_MIN = DOS::Video::HEIGHT * 0.04f;
static const uint16_t Y_MAX = DOS::Video::HEIGHT * 0.96f;
