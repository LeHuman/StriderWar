#include "player.hpp"

#include <dos/graphics.hpp>
#include <math.hpp>
#include <rand.hpp>

#include "world.hpp"

using namespace math;

bool player_situation_update = true;

Player::Player(DOS::Input::Interface *input, situation_t *situation_mem) : input(input), situation(situation_mem) {
    id = -1;
    reset();
}

void Player::reset() {
    damage_queue = 0;
    enabled_bullets = 0;
    last_bullet = false;
    req_bullet = false;
    bounced = 0;
    situation_cycle = 0;
    meltdown_cycle = 0;

    // TODO: Move this reset logic to entity
    ship.entity.x = world::X_CENTER;
    ship.entity.y = world::Y_CENTER;
    ship.entity.vx = 0;
    ship.entity.vy = 0;
    ship.entity.enabled = true;
    ship.entity.has_physics = true;
    ship.entity.color = 1;

    ship.reset();

    for (size_t i = 0; i < MAX_BULLETS; i++) {
        bullets[i].entity.color = 3;
        bullets[i].entity.set_priority(1);
        bullets[i].reset();
    }

    blast_field[6] = &ship.condition.cockpit;
    blast_field[1] = &ship.condition.body;
    blast_field[0] = &ship.condition.thruster.left;
    blast_field[3] = &ship.condition.thruster.right;
    blast_field[9] = &bullets[0].condition.payload;
    blast_field[5] = &bullets[0].condition.body;
    blast_field[2] = &bullets[0].condition.booster;
    blast_field[7] = &bullets[1].condition.payload;
    blast_field[8] = &bullets[1].condition.body;
    blast_field[4] = &bullets[1].condition.booster;
}

static const Fixed bullet_vel_cmp = 1.5f;

void Player::step_damage() {
    ship.step_damage();

    if (meltdown_cycle >= MAX_MELTDOWN_CYCLES) {
        ship.entity.enabled = false;
        if (enabled_bullets) {
            for (size_t i = 0; i < MAX_BULLETS; i++) {
                Bullet &bullet = bullets[i];
                world::explode(bullet);
            }
        }

        bullets[0].entity.x = ship.entity.x;
        bullets[0].entity.y = ship.entity.y;
        bullets[0].mult.damage = 50;
        world::explode(bullets[0]);
    } else if (meltdown_cycle) {
        ++meltdown_cycle;
    } else if (ship.condition.body == Condition::DISABLED) {
        meltdown_cycle = 1;
    }

    if (damage_queue == 0) {
        return;
    }

    int i = random::get(CONDITIONS * 2);
    if (i < CONDITIONS) {
        Condition::damage(blast_field[i]);
    }

    if (--damage_queue == 0) {
        for (size_t i = 0; i < MAX_BULLETS; i++) {
            bullets[i].step_damage();
        }
        ship.damage_roll();
    }
}

void Player::step() {
    if (!ship.entity.enabled) {
        return;
    }

    step_damage();

    // IMPROVE: Actually base thrust boost on orientation?
    Fixed ix = (Fixed)(input->x) / Fixed(300 - ship.mult.left_turn);
    Fixed iy = (Fixed)(input->y) / Fixed(300 - ship.mult.right_turn);

    // TODO: Fix Fixed implementation, there seems to be an offset against positive numbers
    if (ix > 0) {
        ix += 0.08f;
    }

    if (iy > 0) {
        iy += 0.08f;
    }

    ship.entity.pulse(ix, iy);

    req_bullet = input->fire;

    ship.entity.step();

    if (bounced == 0 && (ship.entity.bounce.x || ship.entity.bounce.y)) {
        const Fixed &x = ship.entity.x;
        const Fixed &y = ship.entity.y;

        bounced = 4;
        ship.entity.bounce.x = false;
        ship.entity.bounce.y = false;

        spark.x0 = x;
        spark.y0 = y;
        spark.x1 = spark.x0 + random::get(-8, 8);
        spark.y1 = spark.y0 + random::get(-8, 8);
    }

    bool fire = false;
    bool at_fire_speed = ((ship.entity.vx * ship.entity.vx) + (ship.entity.vy * ship.entity.vy)) > bullet_vel_cmp;
    situation->panel.low_speed.set(at_fire_speed ? P_SS_OFF : P_SS_FAIR);

    if (at_fire_speed && (req_bullet != last_bullet)) {
        fire = req_bullet;
        last_bullet = req_bullet;
    }

    size_t disabled = 0;

    for (size_t i = 0; i < MAX_BULLETS; i++) {
        Bullet &bullet = bullets[i];

        if (!bullet.entity.enabled && bullet.is_disabled()) {
            situation->bullet[i].indicator.set(P_SS_FAIL);
            disabled++;
            continue;
        }

        if (input->alt && bullet.entity.enabled) {
            bullet.entity.bounce.x = true;
        } else if (fire && !bullet.entity.enabled && bullet.loaded) {
            fire = false;
            bullet.loaded = false;
            bullet.entity.enabled = true;
            ++enabled_bullets;
            bullet.entity.x = ship.entity.x;
            bullet.entity.y = ship.entity.y;
            bullet.entity.vx = ship.entity.vx * (world::bullet_speed * bullet.mult.speed);
            bullet.entity.vy = ship.entity.vy * (world::bullet_speed * bullet.mult.speed);
            bullet.fired();
        }

        bullet.step_loading((Condition::HIGH - ship.condition.body) * 50);
        bullet.entity.step();

        if (bullet.entity.bounce.x || bullet.entity.bounce.y) {
            bullet.entity.draw(true);
            bullet.entity.bounce.x = false;
            bullet.entity.bounce.y = false;
            bullet.entity.enabled = false;
            --enabled_bullets;
            world::explode(bullet);
        }

        situation->bullet[i].indicator.set((bullet.loaded && !bullet.entity.enabled) ? P_SS_GOOD : P_SS_FAIR);
    }

    if (disabled >= MAX_BULLETS) {
        situation->panel.gun_ready.set(P_SS_FAIL);
    } else {
        bool loaded = false;
        for (size_t i = 0; i < MAX_BULLETS; i++) {
            bullets[i].entity.set_priority(enabled_bullets + 1);
            loaded |= bullets[i].loaded;
        }
        situation->panel.gun_ready.set(loaded ? P_SS_GOOD : P_SS_FAIR);
    }

    step_situation();
}
