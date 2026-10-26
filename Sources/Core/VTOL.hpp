#pragma once
#include "GameData.hpp"
#include "ItemBox.hpp"
#include <array>
#include <string_view>
namespace heavymach {
// User-approved expansion. Original MCH/MAP readers and definitions stay intact.
struct VTOLSpec {
 const char* name; const char* tile; const char* statsSource; const char* weapon;
 float width,aspect; ObjectBounds bounds; Vector2 muzzle;
 std::array<Vector2,1> exhaust;
};
const std::array<VTOLSpec,3>& vtolSpecs();
const VTOLSpec* vtolSpec(std::string_view name);
void addVTOLExpansion(GameData&);
Vector2 vtolRotate(Vector2,float);
float vtolPitch(float previous,float horizontalSpeed,float verticalSpeed);
}
