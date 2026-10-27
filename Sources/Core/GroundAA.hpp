#pragma once
#include "GameData.hpp"
#include "Visuals.hpp"
#include <array>
#include <optional>
namespace heavymach {
struct GroundAAImage {
 std::string name,source;
 std::array<int,4> crop; // authored pixels, top-down x/y/width/height
 Vector2 size; // cached logical pixels
 float radarAnchorX=0;
};
const std::vector<GroundAAImage>& groundAAImages();
const GroundAAImage* groundAAImage(std::string_view name);
bool isGroundAA(std::string_view name);
std::string groundAAPartTile(std::string_view part,int frame=0);
std::optional<Vector2> groundAAPartSize(std::string_view part);
void configureGroundAADraw(DrawSprite&,const PartsState&,double gameTime);
struct GroundAAPlacement {int mission,stage,originalTanks,shilka,tunguska;std::string map;};
std::vector<GroundAAPlacement> groundAAPlan(const GameData&);
void addGroundAAExpansion(GameData&);
}
