#include "Motion.hpp"
#include <bit>
#include <cmath>
#include <limits>
#include <stdexcept>
namespace heavymach {
namespace {
const GroundSegment* segment(int index, const MotionWorld& w) {
    if (index == -1) return nullptr;
    if (index < 0 || static_cast<size_t>(index) >= w.ground.size())
        throw std::out_of_range("ground segment index");
    return &w.ground[static_cast<size_t>(index)];
}
float bounce(double time, float scale) {
    const double value = time * 50.0; // ARM literal at VA 0x5234 is high word 0x40490000.
    if (!std::isfinite(value) || value > std::numeric_limits<int32_t>::max() || value < std::numeric_limits<int32_t>::min())
        throw std::domain_error("engine bounce time outside restored int32 conversion domain");
    const int32_t integral = static_cast<int32_t>(value);
    const auto signedLow = std::bit_cast<int16_t>(static_cast<uint16_t>(integral));
    return static_cast<float>(signedLow % 10) * scale;
}
Vector2 endPoint(const ColVector& c) {
    return {c.origin.x + c.dir.x * c.length, c.origin.y + c.dir.y * c.length};
}
}
void GobHvMach::setCurrentCol(int index, const MotionWorld& w) {
    const auto* s = segment(index,w);
    prevCol = currentCol;
    currentCol = index;
    // Messages to nil yield +0 for both components; negate x yields -0.
    const Vector2 n = s ? s->collision.normal : Vector2{};
    colRotate = static_cast<float>(std::atan2(-static_cast<double>(n.x), static_cast<double>(n.y)));
}
void GobHvMach::setMoveVel(float input, bool passive3) {
    if (dashVel == 0 && speed >= 0 && input < -0.5f) dashVel = -2.5f;
    speed = input * moveSpeed;
    if (player && passive3) speed = 1.3f * speed;
}
MotionEvents GobHvMach::processLand(const MotionWorld& w) {
    const auto* s = segment(currentCol,w);
    if (!s) { machState = 2; velocity.y = 30; }
    else {
        const float slope = speed < 0 ? -colRotate : colRotate;
        if (slope < -0.7f) { machState = 2; velocity.y = 30; }
        else {
            velocityTick = s->collision.dir;
            if (slope > 0.7f) {
                speed = speed < 0 ? -1.5f : 1.5f;
                velocityTick.mul(speed);
            } else velocityTick.mul(-speed);
            position.add(velocityTick);
        }
        position.sub(s->collision.origin);
        const float along = s->collision.dir.dotProduct(position);
        position.add(s->collision.origin);
        if (along < 0) {
            const int old = currentCol;
            setCurrentCol(s->next,w);
            if (const auto* next = segment(currentCol,w)) position = endPoint(next->collision);
            else if (!player) { setCurrentCol(old,w); position = segment(old,w)->collision.origin; }
        } else if (along > s->collision.length) {
            const int old = currentCol;
            setCurrentCol(s->prev,w);
            if (const auto* prev = segment(currentCol,w)) position = prev->collision.origin;
            else if (!player) { setCurrentCol(old,w); position = endPoint(segment(old,w)->collision); }
        }
    }
    if (std::fabs(colRotate) < 0.7f) rotation = rotation + (colRotate - rotation) * 0.125f;
    if (useEngineBounce) engineBounce = bounce(w.time, speed != 0 ? 0.15f : 0.04f);
    return {};
}
MotionEvents GobHvMach::processFly(const MotionWorld& w) {
    MotionEvents events;
    if (machType != 1) velocity.y = velocity.y - std::bit_cast<float>(uint32_t{0x40e55556});
    velocityTick = velocity;
    velocityTick.mul(std::bit_cast<float>(uint32_t{0x3c888889}));
    velocityTick.x = speed;
    bool collided = false;
    if (machType != 1) {
        for (int index : w.collisionCandidates) {
            const auto* s = segment(index,w);
            if (!s || !s->collision.checkCollideVel(position,velocityTick)) continue;
            setCurrentCol(index,w);
            if (velocity.y < 0 || (speed < 0 ? colRotate > -0.7f : colRotate < 0.7f)) {
                useBooster = 0;
                machState = 1;
            }
            collided = true;
            break;
        }
    }
    if (!collided) {
        position.add(velocityTick);
        if (position.y < w.bottom) events.push_back({MotionEventKind::Damage,hp,0,position});
    }
    rotation = rotation + (speed / -6.0f - rotation) / 12.0f;
    return events;
}
MotionEvents GobHvMach::processModel(const MotionWorld& w) {
    if (speed == 0) { engineBounce = 0; return {}; }
    engineBounce = bounce(w.globalTime,0.1f);
    if (!w.random) return {{MotionEventKind::ModelRandomRequired}};
    const int32_t roll = w.random();
    if (roll < 0) throw std::domain_error("random() must return a nonnegative 31-bit value");
    if (roll % 60 != 0 || !hasWeapon) return {};
    const int32_t aim = w.random();
    if (aim < 0) throw std::domain_error("random() must return a nonnegative 31-bit value");
    const float direction = (static_cast<float>(aim) * std::bit_cast<float>(uint32_t{0x30000000})) * 0.5f;
    return {{MotionEventKind::ModelAim,0,direction,position}};
}
MotionEvents GobHvMach::tick(const MotionWorld& w) {
    MotionEvents events;
    if (!visible) return events;
    if (!w.paused) {
        if (machState == 0) events = processModel(w);
        else {
            if (dashVel != 0) {
                speed = dashVel + speed;
                dashVel = dashVel + (0.0f - dashVel) / 25.0f;
                if (std::fabs(dashVel) < 0.1f) dashVel = 0;
            }
            if (speed < 0 && position.x < w.backLine + 30.0f) speed = 0;
            if (speed > 0 && position.x > w.frontLine - 30.0f) speed = 0;
            if (player && speed > 0 && position.x > w.right - 30.0f) {
                if (w.gameMode != 1 && machState != 3 && w.bossCount < 1) {
                    events.push_back({MotionEventKind::NextStage,0,0,position});
                    machState = 3;
                } else speed = 0;
            }
            MotionEvents step;
            if (machState == 1) step = processLand(w);
            else if (machState == 2) step = processFly(w);
            events.insert(events.end(),step.begin(),step.end());
        }
    }
    events.push_back({MotionEventKind::TickChildren});
    return events;
}
MotionEvents GobHvMach::setBooster(uint8_t grade, bool passive2) {
    if (hp <= 0) return {};
    if (machState != 1) {
        if (machState != 2) return {};
        if (grade == 0 || useBooster != 0) {
            if (grade < 5 || useBooster != 1) return {};
            useBooster = 2;
        } else useBooster = 1;
    }
    machState = 2;
    velocity.y = boosterVel;
    if (passive2) velocity.y = velocity.y * 1.3f;
    // Host must resolve getFreeParticle and the exact random smoke parameters.
    return {{MotionEventKind::BoosterSmoke,0,0,position}};
}
}
