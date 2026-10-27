#pragma once
#include "Geometry.hpp"
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace heavymach {
// Fields follow the original 1.13 readers; host pointers and padding are absent.
struct WeaponInfo {
    std::string name;
    int16_t wpnType = 0, dmg = 0;
    uint8_t pierce = 0, shotCnt = 0;
    float shotDly = 0, delay = 0, spd = 0, spdRand = 0, aimRand = 0, g = 0;
    int16_t charge = 0;
    float liveTm = 0;
    uint8_t rotWpn = 0, uiTile = 0;
    std::string bulletTile;
    int16_t bulletTileNo = 0;
    std::string effectType, effectTile;
    int16_t effectTileNo = 0, shell = 0;
    std::string fireSFX;
    // Approved new aircraft only. Original binary readers leave these defaults.
    float aiTimeScale=1,burstGroupDelay=0;
    uint8_t burstGroupSize=0;
    int effectTypeNumber() const;
};
struct PartsInfo {
    std::string name;
    uint8_t partsType = 0, tileCnt = 0;
    std::vector<Vector2> sockets;
    std::string tileName() const;
};
struct MachPartDefinition {
    std::string name;
    uint8_t baseSocket = 0, zOrder = 0;
    std::vector<MachPartDefinition> children;
};
struct MachDefinition {
    std::string name;
    int32_t hpMax = 0, giveExp = 0;
    uint8_t machType = 0;
    std::vector<MachPartDefinition> parts;
};
struct MapObjectDefinition {
    std::string name; // Original name without .png.
    Vector2 position;
    uint8_t layer = 0;
};
struct EnemyDefinition {
    std::string name;
    Vector2 position;
    bool isBoss = false;
};
struct MapDefinition {
    float left = 0, top = 0, right = 0, bottom = 0;
    std::string background;
    std::vector<MapObjectDefinition> objects;
    std::vector<std::vector<Vector2>> collisionPaths;
    std::vector<EnemyDefinition> enemies; // The original reader ignores Player markers.
    // Segments retain original origin=current joint, second=previous joint order.
    // Call after moving/copying a map; returned links point within the result.
    std::vector<ColVector> collisionVectors() const;
};
class GameData {
public:
    std::unordered_map<std::string, WeaponInfo> weapons;
    std::unordered_map<std::string, PartsInfo> parts;
    std::unordered_map<std::string, MachDefinition> machines;
    std::unordered_map<std::string, MapDefinition> maps; // Stage01 ... Stage42, Survival
    static GameData loadDirectory(const std::filesystem::path& rawDataDirectory);
    static std::vector<WeaponInfo> parseWeapons(std::span<const uint8_t> bytes);
    static std::vector<PartsInfo> parseParts(std::span<const uint8_t> bytes);
    static std::vector<MachDefinition> parseMachines(std::span<const uint8_t> bytes);
    static MapDefinition parseMap(std::span<const uint8_t> bytes);
};
}
