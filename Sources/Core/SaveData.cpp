#include "SaveData.hpp"
#include <algorithm>
#include <cmath>

namespace heavymach {
uint32_t SaveSlot::u32(size_t offset) const {
    if (offset > byteCount - 4) throw std::out_of_range("save field");
    return uint32_t(bytes[offset]) | uint32_t(bytes[offset + 1]) << 8 |
        uint32_t(bytes[offset + 2]) << 16 | uint32_t(bytes[offset + 3]) << 24;
}
void SaveSlot::set32(size_t offset, uint32_t value) {
    if (offset > byteCount - 4) throw std::out_of_range("save field");
    for (size_t i = 0; i < 4; ++i) bytes[offset + i] = uint8_t(value >> (i * 8));
}
std::string SaveSlot::weaponName(size_t index) const {
    if (index >= weaponCapacity) throw std::out_of_range("weapon index");
    const auto start = bytes.begin() + 0x34 + index * 0x1c;
    const auto end = std::find(start, start + 24, uint8_t(0));
    if (end == start + 24) throw std::runtime_error("unterminated weapon name");
    return std::string(start, end);
}
int32_t SaveSlot::weaponCharge(size_t index) const {
    if (index >= weaponCapacity) throw std::out_of_range("weapon index");
    return i32(0x4c + index * 0x1c);
}
void SaveSlot::setWeapon(size_t index, std::string_view name, int32_t charge) {
    if (index >= weaponCapacity || name.size() >= 24) throw std::out_of_range("weapon record");
    auto start = bytes.begin() + 0x34 + index * 0x1c;
    std::fill(start, start + 24, 0);
    std::copy(name.begin(), name.end(), start);
    setInt(0x4c + index * 0x1c, charge);
}
void SaveSlot::validate() const {
    if (!occupied()) return;
    if (level() < 0 || level() > 99 || weaponCount() < 0 ||
        weaponCount() > int32_t(weaponCapacity) || (!std::isfinite(f32(0x24)) && level()<67))
        throw std::runtime_error("invalid save slot");
    for (int32_t i = 0; i < weaponCount(); ++i) (void)weaponName(i);
}
SaveData SaveData::decode(std::span<const uint8_t> bytes) {
    // Original NSData path assumes this size. Reject truncation instead of
    // reading beyond the buffer on modern platforms. No silent save reset.
    if (bytes.size() != byteCount) throw std::runtime_error("HvM.dat must be 3624 bytes");
    SaveData result;
    for (size_t i = 0; i < 3; ++i) {
        std::copy_n(bytes.begin() + i * SaveSlot::byteCount, SaveSlot::byteCount,
                    result.slots[i].bytes.begin());
        result.slots[i].validate();
    }
    return result;
}
std::array<uint8_t, SaveData::byteCount> SaveData::encode() const {
    std::array<uint8_t, byteCount> result;
    for (size_t i = 0; i < 3; ++i) {
        slots[i].validate();
        std::copy(slots[i].bytes.begin(), slots[i].bytes.end(), result.begin() + i * SaveSlot::byteCount);
    }
    return result;
}
}
