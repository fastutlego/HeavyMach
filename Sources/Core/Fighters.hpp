#pragma once
#include "VTOL.hpp"
namespace heavymach {
// Uses the same sprite, attachment and hitbox path as the approved VTOLs.
const std::array<VTOLSpec,3>& fighterSpecs();
const VTOLSpec* aircraftSpec(std::string_view name);
struct FighterWave {
 const char* map; const char* boss; // Empty boss: final approach to the next boss stage.
 std::array<int,3> counts;
 std::array<Vector2,8> positions; // Relative world X/Y, shared by spawn and entry destinations.
};
const std::array<FighterWave,14>& fighterWaves();
const FighterWave* fighterWave(std::string_view map);
void addFighterExpansion(GameData&);
}
