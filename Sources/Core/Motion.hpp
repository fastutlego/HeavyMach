#pragma once
#include "Geometry.hpp"
#include <cstdint>
#include <functional>
#include <span>
#include <vector>
namespace heavymach {
struct GroundSegment { ColVector collision; int prev = -1, next = -1; };
struct MotionWorld {
    std::span<const GroundSegment> ground;
    std::span<const int> collisionCandidates; // Original ordered getColListFromX result; empty means no candidates.
    float backLine = 0, frontLine = 0, right = 0, bottom = 0;
    int bossCount = 0, gameMode = 0;
    double time = 0, globalTime = 0;
    bool paused = false;
    // Original random() returns a nonnegative 31-bit value. Host supplies it.
    std::function<int32_t()> random;
};
enum class MotionEventKind { Damage, NextStage, BoosterSmoke, ModelAim, ModelRandomRequired, TickChildren };
struct MotionEvent {
    MotionEventKind kind;
    int amount = 0;
    float value = 0;
    Vector2 position{};
};
using MotionEvents = std::vector<MotionEvent>;
class GobHvMach {
public:
    Vector2 position{}, velocity{}, velocityTick{};
    float speed = 0, moveSpeed = 1.4f, dashVel = 0, boosterVel = 230;
    float rotation = 0, colRotate = 0, engineBounce = 0;
    int hp = 256;
    int16_t machState = 0, useBooster = 0;
    uint8_t machType = 3;
    bool player = false, visible = true, useEngineBounce = false, hasWeapon = false;
    int currentCol = -1, prevCol = -1;
    void setCurrentCol(int index, const MotionWorld& world);
    void setMoveVel(float input, bool passive3 = false);
    MotionEvents tick(const MotionWorld& world);
    MotionEvents processLand(const MotionWorld& world);
    MotionEvents processFly(const MotionWorld& world);
    MotionEvents processModel(const MotionWorld& world);
    // Player subclass 0x1cc70. Original BOOL argument does not gate behavior.
    MotionEvents setBooster(uint8_t trackGrade, bool passive2 = false);
};
}
