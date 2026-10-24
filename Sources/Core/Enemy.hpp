#pragma once
#include "Motion.hpp"
namespace heavymach {
struct MachDefinition;
struct EnemyFrame {
    MotionWorld motion;
    Vector2 playerPos{};
    int32_t mission = 0, internalLevel = 0;
};
enum class EnemyEventKind { PartsDefinition, GaugeCreate, GaugeUpdate, KillBoss, Explosion, AddExperience, Remove, Sound };
struct EnemyEvent {
    EnemyEventKind kind;
    Vector2 position{};
    int32_t amount = 0;
    float value = 0, pan = 0, scale = 1, life = 0;
    int32_t tileNo = 0, animationFrames = 0, blend = 0;
    const char* asset = nullptr;
};
struct EnemyHost {
    virtual ~EnemyHost() = default;
    virtual int32_t random31() = 0;
    virtual bool acquireParticle() = 0;
    // Synchronous: original GobItemBox init may consume the shared RNG stream.
    virtual void spawnItemBox(Vector2 position) = 0;
    virtual void event(const EnemyEvent&) = 0;
};
class GobHvM_Enemy : public GobHvMach {
public:
    GobHvM_Enemy();
    bool isBoss = false, hasHPGauge = false;
    int32_t giveItem = 0, giveExp = 0, hpMax = 0;
    float speedDest = 0, gravityDest = 0, gaugeSize = 0, gaugeY = 0;
    bool makeMachFromDefinition(const MachDefinition&, EnemyHost&);
    MotionEvents tick(const EnemyFrame&, EnemyHost&);
    void onDamage(int32_t amount, const EnemyFrame&, EnemyHost&);
    void onDestroy(const EnemyFrame&, EnemyHost&);
    void setHPGauge(float fraction, EnemyHost&) const;
};
}
