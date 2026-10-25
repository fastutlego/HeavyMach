#pragma once
#include "SaveData.hpp"
#include <optional>

namespace heavymach {
struct ExperienceThreshold { int32_t previous, next; };
struct ExperienceChange { bool leveledUp; float gauge; };
struct StagePosition { int32_t mission, stage; };
class GlobalInfo {
public:
    explicit GlobalInfo(SaveSlot& slot) : slot_(slot) {}
    void selectSlot(int32_t unixTime);
    static ExperienceThreshold thresholds(int32_t internalLevel);
    ExperienceChange addExperience(int32_t amount);
    size_t addWeaponItem(std::string_view name, int32_t charge);
    std::optional<size_t> selectWeapon(int32_t index) const;
    bool removeWeapon(size_t index);
    static int32_t stageCount(int32_t mission);
    static int32_t currentStage(StagePosition position);
    static int32_t mapNumber(StagePosition position);
    bool updateLastStage(StagePosition position);
    static StagePosition nextStage(StagePosition position);
    static int32_t maximumHP(uint8_t topGrade);
    static float movementSpeed(uint8_t bodyGrade);
private:
    SaveSlot& slot_;
};
}
