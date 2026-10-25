#pragma once
#include <array>
#include <bit>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

namespace heavymach {
// ARMv6 HvM.dat layout, from GameMain::selectSlot (0x3b20), openDataFile
// (0x3ea0), saveDataFile (0x3f44). Explicit little-endian fields preserve the
// original file independently of native pointer size and compiler padding.
class SaveSlot {
public:
    static constexpr size_t byteCount = 0x4b8;
    static constexpr size_t weaponCapacity = (0x3b4 - 0x34) / 0x1c;
    std::array<uint8_t, byteCount> bytes{};
    uint32_t u32(size_t offset) const;
    int32_t i32(size_t offset) const { return std::bit_cast<int32_t>(u32(offset)); }
    float f32(size_t offset) const { return std::bit_cast<float>(u32(offset)); }
    void set32(size_t offset, uint32_t value);
    void setInt(size_t offset, int32_t value) { set32(offset, std::bit_cast<uint32_t>(value)); }
    void setFloat(size_t offset, float value) { set32(offset, std::bit_cast<uint32_t>(value)); }
    bool occupied() const { return bytes[8] != 0; }
    int32_t experience() const { return i32(0x20); }
    int32_t level() const { return i32(0x28); } // Internal zero-based level.
    int32_t weaponCount() const { return i32(0x30); }
    std::string weaponName(size_t index) const;
    int32_t weaponCharge(size_t index) const;
    void setWeapon(size_t index, std::string_view name, int32_t charge);
    void validate() const;
};

class SaveData {
public:
    static constexpr size_t byteCount = 3 * SaveSlot::byteCount;
    std::array<SaveSlot, 3> slots{};
    static SaveData decode(std::span<const uint8_t> bytes);
    std::array<uint8_t, byteCount> encode() const;
};
}
