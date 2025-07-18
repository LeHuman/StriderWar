#include "player.hpp"

#include <dos/graphics.hpp>
#include <dos/math.hpp>
#include <rand.hpp>

#include "world.hpp"

Bullet::Bullet() : loaded(true), delay(MAX_DELAY) {
    condition.payload = Condition::HIGH;
    condition.body = Condition::HIGH;
    condition.booster = Condition::HIGH;

    step_damage();
}

Ship::Ship() : breach(false), pressure(MAX_PRESSURE), inferno(0), update_cycle(0) {
    condition.cockpit = Condition::HIGH;
    condition.body = Condition::HIGH;
    condition.thruster.left = Condition::HIGH;
    condition.thruster.right = Condition::HIGH;

    step_damage();
}

Player::Player(DOS::Input::Interface &input, situation_t &situation_mem) : id(-1), damage_queue(0), enabled_bullets(0), input(input), last_bullet(false), req_bullet(false), bounced(0), situation(situation_mem), situation_cycle(0), meltdown_cycle(0) {
    ship.entity.x = world::X_CENTER;
    ship.entity.y = world::Y_CENTER;
    ship.entity.enabled = true;
    ship.entity.has_physics = true;
    ship.entity.color = 1;

    for (size_t i = 0; i < MAX_BULLETS; i++) {
        bullets[i].entity.color = 3;
        bullets[i].entity.set_priority(1);
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

void Player::damage(int hits) {
    damage_queue += hits;
}

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

void Player::step_situation() {
    switch (situation_cycle++ % 20) { // MAX_SITUATIONS?
        case 0:
            situation.ship.pilot.set(ship.condition.cockpit);
            break;
        case 1:
            situation.ship.body.set(ship.condition.body);
            break;
        case 2:
            situation.ship.thruster.left.set(ship.condition.thruster.left);
            break;
        case 3:
            situation.ship.thruster.right.set(ship.condition.thruster.right);
            break;
        case 4:
            situation.bullet[0].head.set(bullets[0].condition.payload);
            break;
        case 5:
            situation.bullet[0].body.set(bullets[0].condition.body);
            break;
        case 6:
            situation.bullet[0].boost.set(bullets[0].condition.booster);
            break;
        case 7:
            situation.bullet[1].head.set(bullets[1].condition.payload);
            break;
        case 8:
            situation.bullet[1].body.set(bullets[1].condition.body);
            break;
        case 9:
            situation.bullet[1].boost.set(bullets[1].condition.booster);
            break;
        case 10:
            situation.panel.ammo_low.set(Condition::disable_high((Condition::T)((bullets[0].condition.payload + bullets[1].condition.payload + 1) / 2)));
            break;
        case 11:
            situation.panel.auto_pilot.set(ship.auto_pilot_forced() ? P_SS_FAIL : P_SS_OFF);
            break;
        case 12:
            situation.panel.booster_damage.set(Condition::disable_high((Condition::T)((bullets[0].condition.booster + bullets[1].condition.booster + 1) / 2)));
            break;
        case 13:
            situation.panel.dead.set(ship.is_disabled() ? P_SS_FAIL : P_SS_OFF);
            break;
        case 14:
            situation.panel.fire.set(ship.inferno ? (ship.inferno > (ship.MAX_INFERNO / 2) ? P_SS_FAIL : P_SS_FAIR) : P_SS_OFF);
            break;
        case 15:
            situation.panel.fuel_low.set(Condition::disable_high((Condition::T)((bullets[0].condition.body + bullets[1].condition.body + 1) / 2)));
            break;
        case 16:
            situation.panel.hull_breach.set(ship.pressure < (ship.MAX_PRESSURE / 3) ? P_SS_FAIL : (ship.pressure < ((ship.MAX_PRESSURE * 2) / 3) ? P_SS_FAIR : (ship.pressure == ship.MAX_PRESSURE ? P_SS_OFF : P_SS_GOOD)));
            break;
        case 17:
            situation.panel.left_authority.set(ship.condition.thruster.left);
            break;
        case 18:
            situation.panel.right_authority.set(ship.condition.thruster.right);
            break;
        case 19:
            situation.panel.nuclear_meltdown.set(meltdown_cycle > ((MAX_MELTDOWN_CYCLES * 2) / 3) ? P_SS_FAIL : (meltdown_cycle > (MAX_MELTDOWN_CYCLES / 3) ? P_SS_FAIR : (meltdown_cycle == 0 ? P_SS_OFF : P_SS_GOOD)));
            break;
        // TODO: Update remaining situations
        default:
            // Not updated here
            break;
    }
}

void Player::step() {
    if (!ship.entity.enabled) {
        return;
    }

    step_damage();

    // IMPROVE: Actually base thrust boost on orientation?
    Fixed ix = (Fixed)(input.x) / Fixed(300 - ship.mult.left_turn);
    Fixed iy = (Fixed)(input.y) / Fixed(300 - ship.mult.right_turn);

    ship.entity.pulse(ix, iy);

    req_bullet = input.fire;

    ship.entity.step();

    if (bounced == 0 && (ship.entity.bounce.x || ship.entity.bounce.y)) {
        bounced = 4;
        ship.entity.bounce.x = false;
        ship.entity.bounce.y = false;

        static const Fixed spark_dist = 0.25f;
        static const Fixed spark_tail = -0.5f;
        static const Fixed spark_deflect_rad = 30.0f;

        if (ship.entity.bounce.angle < DOS::Math::Fix::PI) {
            ship.entity.bounce.angle -= spark_deflect_rad;
        } else {
            ship.entity.bounce.angle += spark_deflect_rad;
        }

        Fixed dx = DOS::Math::Fix::cos(ship.entity.bounce.angle);
        Fixed dy = DOS::Math::Fix::sin(ship.entity.bounce.angle);

        spark.x0 = ship.entity.x - (dx * spark_dist);
        spark.y0 = ship.entity.y - (dy * spark_dist);
        spark.x1 = spark.x0 + (uint16_t)(dx * spark_tail);
        spark.y1 = spark.y0 + (uint16_t)(dy * spark_tail);
    }

    bool fire = false;
    bool at_fire_speed = ((ship.entity.vx * ship.entity.vx) + (ship.entity.vy * ship.entity.vy)) > bullet_vel_cmp;
    situation.panel.low_speed.set(at_fire_speed ? P_SS_OFF : P_SS_FAIR);

    if (at_fire_speed && (req_bullet != last_bullet)) {
        fire = req_bullet;
        last_bullet = req_bullet;
    }

    size_t disabled = 0;

    for (size_t i = 0; i < MAX_BULLETS; i++) {
        Bullet &bullet = bullets[i];

        if (!bullet.entity.enabled && bullet.is_disabled()) {
            situation.bullet[i].indicator.set(P_SS_FAIL);
            disabled++;
            continue;
        }

        if (input.alt && bullet.entity.enabled) {
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

        situation.bullet[i].indicator.set((bullet.loaded && !bullet.entity.enabled) ? P_SS_GOOD : P_SS_FAIR);
    }

    if (disabled >= MAX_BULLETS) {
        situation.panel.gun_ready.set(P_SS_FAIL);
    } else {
        bool loaded = false;
        for (size_t i = 0; i < MAX_BULLETS; i++) {
            bullets[i].entity.set_priority(enabled_bullets + 1);
            loaded |= bullets[i].loaded;
        }
        situation.panel.gun_ready.set(loaded ? P_SS_GOOD : P_SS_FAIR);
    }

    step_situation();
}

void Player::draw() {
    if (bounced) {
        DOS::Draw::line(spark, bounced - 1);
        if (bounced == 4) {
            bounced = 1;
        } else {
            bounced = 0;
        }
    }

    ship.entity.draw();
    for (size_t i = 0; i < sizeof(bullets) / sizeof(bullets[0]); i++) {
        bullets[i].entity.draw();
    }
}
