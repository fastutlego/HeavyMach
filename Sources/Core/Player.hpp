#pragma once
#include "Motion.hpp"
#include "Combat.hpp"
#include "GlobalInfo.hpp"
#include <array>
#include <string_view>
namespace heavymach {
struct LootDefinition { int16_t type,weight; std::string_view name; };
std::span<const LootDefinition> originalLootTable();
enum class PlayerEventKind { Sound,HPGauge,ExperienceGauge,Passive,Parts,WeaponInventory,PickupParticle,UpgradeOpen,UpgradeSelection,UpgradeClose,GameOver,Quake };
struct PlayerEvent {
    PlayerEventKind kind;
    int32_t index=0,amount=0;
    float value=0,life=0,scaleVelocity=0,delay=0;
    Vector2 position{},velocity{};
    std::string_view name{};
    bool active=false;
};
struct PlayerHost {
    virtual ~PlayerHost()=default;
    virtual int32_t random31()=0;
    virtual bool acquireParticle()=0;
    virtual void event(const PlayerEvent&)=0;
};
struct PassiveSlot { bool active=false; float time=0; int32_t number=0; };
class PlayerPassives {
public:
    std::array<PassiveSlot,4> slots{};
    bool active(int index) const;
    void add(std::string_view name,PlayerHost&);
    // Called once per GuiGame tick, including paused world ticks.
    void tick(PlayerHost* = nullptr);
    void tick(PlayerHost& host){tick(&host);}
};
class GobHvM_Player : public GobHvMach {
    PlayerPassives ownedPassives_;
public:
    // Original GuiGame owns these slots across player/map/screen replacement.
    // Standalone unit consumers can retain a private fallback.
    PlayerPassives& passives;
    // Gameplay screen 6 creates the gauge; menu previews must set false.
    bool hasHPGauge=true;
    int upgradeSelection=0,upgradeStep=0;
    // Host convenience: binds selected slot, loads slot HP and movement speed.
    explicit GobHvM_Player(SaveSlot&,PlayerPassives* sharedPassives=nullptr);
    GobHvM_Player(const GobHvM_Player&)=delete;
    GobHvM_Player& operator=(const GobHvM_Player&)=delete;
    GobHvM_Player(GobHvM_Player&&)=delete;
    GobHvM_Player& operator=(GobHvM_Player&&)=delete;
    void pickupItem(const GameData&,PlayerHost&);
    void addExperience(int32_t,PlayerHost&);
    void openUpgrade(PlayerHost&);
    bool selectUpgrade(int part,PlayerHost&);
    void confirmUpgrade(PlayerHost&); // UI PopButton 1300 while paused.
    bool upgradeParts(int part,PlayerHost&); // Does not decrement slot byte 0x2c.
    void refreshMaxHp(PlayerHost&);
    void refreshMoveSpeed();
    void onDamage(int32_t,PlayerHost&);
    void setHPGauge(float,PlayerHost&) const;
    void setTap(Vector2 touch,int state,Vector2 playerScreen,Vector2 weaponRotPos,WeaponState*);
private:
    SaveSlot& slot_;
};
}
