#include "GlobalInfo.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace heavymach {
namespace {
// GlobalInfo::init 0xfd44. Mission 4 uses maps 25..29 before 15..24.
constexpr std::array<int32_t, 8> counts{4,5,5,5,5,5,6,6};
constexpr std::array<int32_t, 41> maps{
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,25,26,27,28,29,
    15,16,17,18,19,20,21,22,23,24,30,31,32,33,34,35,36,37,38,39,40,41};
// ARMv6 __fixdfsivfp -> VCVT.S32.F64: round toward zero, saturate.
// See Tests/HighLevelEvidence.md for helper disassembly and ARM VFP table.
int32_t armVFPInt(double value) {
    if (std::isnan(value)) return 0;
    if (value>=double(INT32_MAX)) return INT32_MAX;
    if (value<=double(INT32_MIN)) return INT32_MIN;
    return static_cast<int32_t>(value);
}
int32_t wrapAdd(int32_t a,int32_t b) {return std::bit_cast<int32_t>(uint32_t(a)+uint32_t(b));}
int32_t wrapSub(int32_t a,int32_t b) {return std::bit_cast<int32_t>(uint32_t(a)-uint32_t(b));}
int32_t checkedInt(double value) {
    if (!std::isfinite(value) || value < std::numeric_limits<int32_t>::min() ||
        value > std::numeric_limits<int32_t>::max()) throw std::overflow_error("original 32-bit field overflow");
    return static_cast<int32_t>(value);
}
}
void GlobalInfo::selectSlot(int32_t unixTime) {
    // GameMain::selectSlot: 0x3b20. A new slot starts at INTERNAL level zero.
    if (!slot_.occupied()) {
        slot_.bytes.fill(0); slot_.bytes[8]=1; slot_.setInt(0,unixTime);
        slot_.setInt(0x1c,256); slot_.bytes[0x3b4]=9;
        addWeaponItem("P_Wpn_Gun01",99); addWeaponItem("P_Wpn_RaderBomb",10);
        addWeaponItem("P_Wpn_RaderMsl",10); addWeaponItem("P_Wpn_RaderLsr",5);
    }
    slot_.setInt(4,unixTime);
    if (slot_.i32(0x1c)<1) slot_.setInt(0x1c,256);
    int16_t regen=std::bit_cast<int16_t>(uint16_t(uint16_t(slot_.bytes[0x3b4]) | uint16_t(slot_.bytes[0x3b5])<<8));
    regen=std::clamp<int16_t>(regen,12,22);
    slot_.bytes[0x3b4]=uint8_t(regen); slot_.bytes[0x3b5]=uint8_t(regen>>8);
    if (slot_.i32(0xc)>6) {slot_.setInt(0xc,6);slot_.setInt(0x10,0);}
    (void)thresholds(slot_.level());
    updateLastStage({slot_.i32(0xc),slot_.i32(0x10)});
}
ExperienceThreshold GlobalInfo::thresholds(int32_t level) {
    if (level < 0 || level > 99) throw std::out_of_range("level");
    // 0x105f8: float base is reduced AFTER every iteration, including level 0.
    // Each sum truncates to int, rather than truncating only the final total.
    ExperienceThreshold result{0,0};
    float base = 1.5f;
    for (int32_t i = 0; i <= level; ++i) {
        result.previous = result.next;
        result.next = armVFPInt(double(result.next) + 300.0 * std::pow(double(base), double(i)));
        if (base > 1.25f) base -= 0.01f;
    }
    return result;
}
ExperienceChange GlobalInfo::addExperience(int32_t amount) {
    // 0x106e0: only ONE level per call; excess EXP remains in the total.
    auto threshold = thresholds(slot_.level());
    slot_.setInt(0x20, wrapAdd(slot_.experience(),amount));
    bool levelUp = slot_.level() < 99 && slot_.experience() >= threshold.next;
    if (levelUp) {
        slot_.setInt(0x28, slot_.level() + 1);
        slot_.bytes[0x2c] = uint8_t(slot_.bytes[0x2c] + 1);
        threshold = thresholds(slot_.level());
    }
    float gauge = float(wrapSub(slot_.experience(),threshold.previous)) / float(wrapSub(threshold.next,threshold.previous));
    gauge = std::min(gauge, 1.0f);
    slot_.setFloat(0x24, gauge);
    return {levelUp, gauge}; // Host triggers setUpgrade / Game_LevelUp sound.
}
size_t GlobalInfo::addWeaponItem(std::string_view name, int32_t charge) {
    // 0x1082c: duplicate names accumulate, capped at SIX times this pickup's charge.
    slot_.validate();
    int32_t index = 0;
    for (; index < slot_.weaponCount(); ++index) if (slot_.weaponName(index) == name) break;
    int32_t total = charge;
    if (index < slot_.weaponCount()) total = wrapAdd(slot_.weaponCharge(index), charge);
    total = std::min(total, std::bit_cast<int32_t>(uint32_t(charge) * 6u));
    if (size_t(index) >= SaveSlot::weaponCapacity || name.size() >= 24)
        throw std::out_of_range("weapon record");
    // Original strcpy writes through NUL only, retaining stale record bytes.
    auto destination=slot_.bytes.begin()+0x34+size_t(index)*0x1c;
    std::copy(name.begin(),name.end(),destination); destination[name.size()]=0;
    slot_.setInt(0x4c+size_t(index)*0x1c,total);
    if (index == slot_.weaponCount()) slot_.setInt(0x30, index + 1);
    return size_t(index);
}
std::optional<size_t> GlobalInfo::selectWeapon(int32_t index) const {
    // 0x108ec. Guard malformed/empty inputs; normal game indices are nonnegative.
    if (slot_.weaponCount() <= 0 || index < 0 || index > INT16_MAX) return std::nullopt;
    return size_t(index % slot_.weaponCount());
}
bool GlobalInfo::removeWeapon(size_t index) {
    // 0x10944 uses (newCount * 28 - index) bytes, not a record count.
    // touchHLE libc/generic_char.rs implements guest memcpy via memmove.
    // Preserve the original trailing-byte mutations without host memcpy UB.
    if (index == 0 || index >= size_t(slot_.weaponCount())) return false;
    const size_t newCount=size_t(slot_.weaponCount()-1);
    const size_t destination=0x34+index*0x1c, length=newCount*0x1c-index;
    if (destination+0x1c+length>slot_.bytes.size())
        throw std::out_of_range("original weapon removal exceeds save slot");
    slot_.setInt(0x30,int32_t(newCount));
    std::memmove(slot_.bytes.data()+destination,slot_.bytes.data()+destination+0x1c,length);
    return true;
}
int32_t GlobalInfo::stageCount(int32_t mission) {
    if (mission < 0 || mission >= int32_t(counts.size())) throw std::out_of_range("mission");
    return counts[mission];
}
int32_t GlobalInfo::currentStage(StagePosition p) {
    if (p.stage < 0 || p.stage >= stageCount(p.mission)) throw std::out_of_range("stage");
    int32_t result = p.stage;
    for (int32_t i = 0; i < p.mission; ++i) result += counts[i];
    return result;
}
int32_t GlobalInfo::mapNumber(StagePosition p) { return maps.at(currentStage(p)); }
bool GlobalInfo::updateLastStage(StagePosition p) {
    // 0x10a0c: a replay of an earlier stage must not overwrite unlocked progress.
    int32_t current = currentStage(p);
    if (slot_.i32(0x14) >= current) return false;
    slot_.setInt(0x14,current); slot_.setInt(0xc,p.mission); slot_.setInt(0x10,p.stage);
    return true;
}
StagePosition GlobalInfo::nextStage(StagePosition p) {
    // 0xb8f0: the original final transition clamps to mission 6 / stage 0.
    if (++p.stage >= stageCount(p.mission)) { ++p.mission; p.stage = 0; }
    if (p.mission > 6) return {6,0};
    return p;
}
int32_t GlobalInfo::maximumHP(uint8_t grade) {
    // 0x1cf58: promoted float 1.3f, not the double literal 1.3.
    int32_t hp = 256;
    for (int i = 1; i < grade; ++i) hp = checkedInt(double(hp) + 50.0 * std::pow(double(1.3f), double(i)));
    return hp;
}
float GlobalInfo::movementSpeed(uint8_t grade) {
    // 0x1d0dc, operation order and float precision preserved.
    return ((float(grade) * 0.5f) * 0.3f) + 1.3f;
}
}
