#pragma once
#if defined(HM_VALIDATION) && HM_VALIDATION
#include "GameData.hpp"
#include "SaveData.hpp"
#include <array>
#include <optional>
namespace heavymach {
struct ValidationScenario {
 int mission=0,stage=0,internalLevel=0;
 std::array<uint8_t,3> grades{0,0,0}; // body,top,weapon
 bool nearLevelUp=false,allWeapons=false;
 int selectedWeapon=0;
 std::vector<std::string> weapons; // Optional P_ weapon names; original definition charge.
 std::optional<Vector2> playerPosition;
 std::optional<size_t> bossIndex; // Index in ValidationStage.bosses; position overrides this.
};
struct ValidationStage {
 int mission=0,stage=0;std::string mapName;
 bool naturalProgression=true;
 std::vector<EnemyDefinition> bosses;
};
std::vector<ValidationStage> validationStages(const GameData&);
std::vector<std::string> validationWeapons(const GameData&);
std::optional<Vector2> validationSpawnPosition(const GameData&,const ValidationScenario&);
// Resets only supplied validation slot, preserves all gameplay definitions.
// Validates requested map/grades/weapons before committing the prepared slot.
void prepareValidationSlot(SaveSlot&,const GameData&,const ValidationScenario&);
}
#endif
