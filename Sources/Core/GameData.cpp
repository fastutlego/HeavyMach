#include "GameData.hpp"
#include <algorithm>
#include <bit>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace heavymach {
namespace {
struct Reader {
    std::span<const uint8_t> data;
    void require(size_t at, size_t size) const {
        if (at > data.size() || size > data.size() - at)
            throw std::runtime_error("Truncated HeavyMach resource at byte " + std::to_string(at));
    }
    uint8_t u8(size_t at) const { require(at, 1); return data[at]; }
    uint16_t u16(size_t at) const { require(at, 2); return uint16_t(data[at]) | uint16_t(data[at+1]) << 8; }
    int16_t i16(size_t at) const { return std::bit_cast<int16_t>(u16(at)); }
    uint32_t u32(size_t at) const {
        require(at, 4);
        return uint32_t(data[at]) | uint32_t(data[at+1]) << 8 | uint32_t(data[at+2]) << 16 | uint32_t(data[at+3]) << 24;
    }
    int32_t i32(size_t at) const { return std::bit_cast<int32_t>(u32(at)); }
    float f32(size_t at) const { return std::bit_cast<float>(u32(at)); }
    std::string text(size_t at, size_t size) const {
        require(at, size);
        auto field = data.subspan(at, size);
        auto end = std::find(field.begin(), field.end(), uint8_t(0));
        if (end == field.end()) throw std::runtime_error("Unterminated resource name");
        return std::string(field.begin(), end);
    }
    Vector2 point(size_t at) const { return {f32(at), f32(at+4)}; }
};
struct Node {
    uint32_t type;
    Reader payload;
    std::vector<Node> children;
};
Node readNode(const Reader& reader, size_t& cursor, size_t depth) {
    if (depth > 32) throw std::runtime_error("Resource nesting exceeds limit");
    reader.require(cursor, 12);
    // The first word is a stale serialized size in some maps. The native
    // traversal follows payload size and child count, so do not seek by it.
    uint32_t type = reader.u32(cursor + 4), size = reader.u32(cursor + 8);
    cursor += 12;
    reader.require(cursor, size);
    Node node{type, Reader{reader.data.subspan(cursor, size)}, {}};
    cursor += size;
    uint32_t count = reader.u32(cursor);
    cursor += 4;
    if (count > (reader.data.size() - cursor) / 16)
        throw std::runtime_error("Invalid resource child count");
    node.children.reserve(count);
    for (uint32_t i = 0; i < count; ++i) node.children.push_back(readNode(reader, cursor, depth + 1));
    return node;
}
Node root(std::span<const uint8_t> bytes) {
    size_t cursor = 0;
    auto node = readNode(Reader{bytes}, cursor, 0);
    if (cursor != bytes.size()) throw std::runtime_error("Trailing bytes in HeavyMach resource");
    return node;
}
void expect(const Node& n, uint32_t type, size_t size) {
    if (n.type != type || n.payload.data.size() != size)
        throw std::runtime_error("Unexpected HeavyMach record layout");
}
MachPartDefinition part(const Node& n) {
    expect(n, 4, 26);
    MachPartDefinition result{n.payload.text(0,24), n.payload.u8(24), n.payload.u8(25), {}};
    for (const auto& child : n.children) result.children.push_back(part(child));
    return result;
}
std::vector<uint8_t> readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot read resource: " + path.string());
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}
template <typename T> void insert(std::unordered_map<std::string,T>& target, std::vector<T> records) {
    for (auto& value : records) {
        auto name = value.name;
        if (!target.emplace(name, std::move(value)).second)
            throw std::runtime_error("Duplicate resource name: " + name);
    }
}
}

int WeaponInfo::effectTypeNumber() const {
    if (effectType == "Normal") return 1;
    if (effectType == "Flame") return 2;
    if (effectType == "Smoke") return 3;
    if (effectType == "Trail") return 4;
    if (effectType == "Laser") return 5;
    return 0;
}
std::string PartsInfo::tileName() const { return tileCnt == 0 ? "PartsDummy.png" : name + ".png"; }

std::vector<WeaponInfo> GameData::parseWeapons(std::span<const uint8_t> bytes) {
    auto tree = root(bytes);
    std::vector<WeaponInfo> result;
    for (const auto& n : tree.children) {
        expect(n, 1, 156);
        const auto& r = n.payload;
        WeaponInfo w;
        w.name=r.text(0,24); w.wpnType=r.i16(24); w.dmg=r.i16(26);
        w.pierce=r.u8(28); w.shotCnt=r.u8(29); w.shotDly=r.f32(30);
        w.delay=r.f32(34); w.spd=r.f32(38); w.spdRand=r.f32(42);
        w.aimRand=r.f32(46); w.g=r.f32(50); w.charge=r.i16(54);
        w.liveTm=r.f32(56); w.rotWpn=r.u8(60); w.uiTile=r.u8(61);
        w.bulletTile=r.text(62,24); w.bulletTileNo=r.i16(86);
        w.effectType=r.text(88,16); w.effectTile=r.text(104,24);
        w.effectTileNo=r.i16(128); w.shell=r.i16(130); w.fireSFX=r.text(132,24);
        result.push_back(std::move(w));
    }
    return result;
}
std::vector<PartsInfo> GameData::parseParts(std::span<const uint8_t> bytes) {
    auto tree = root(bytes);
    std::vector<PartsInfo> result;
    for (const auto& n : tree.children) {
        const auto& r = n.payload;
        auto count = r.u8(26);
        expect(n, 1, 27 + size_t(count)*8);
        PartsInfo p{r.text(0,24),r.u8(24),r.u8(25),{}};
        for (uint8_t i=0; i<count; ++i) p.sockets.push_back(r.point(27+size_t(i)*8));
        result.push_back(std::move(p));
    }
    return result;
}
std::vector<MachDefinition> GameData::parseMachines(std::span<const uint8_t> bytes) {
    auto tree = root(bytes);
    std::vector<MachDefinition> result;
    for (const auto& n : tree.children) {
        expect(n, 3, 33);
        const auto& r = n.payload;
        MachDefinition m{r.text(0,24),r.i32(24),r.i32(28),r.u8(32),{}};
        for (const auto& child : n.children) m.parts.push_back(part(child));
        result.push_back(std::move(m));
    }
    return result;
}
MapDefinition GameData::parseMap(std::span<const uint8_t> bytes) {
    auto tree = root(bytes);
    MapDefinition map;
    bool bounds = false, background = false;
    for (const auto& group : tree.children) {
        const auto& r = group.payload;
        switch (group.type) {
        case 2:
            expect(group,2,16);
            map.left=r.f32(0); map.top=r.f32(4); map.right=r.f32(8); map.bottom=r.f32(12);
            bounds=true;
            break;
        case 7:
            for (const auto& n : group.children) {
                const auto& data=n.payload;
                auto length=data.u32(0);
                expect(n,8,4+size_t(length)+9);
                map.objects.push_back({data.text(4,length),data.point(4+length),uint8_t(data.u8(12+length) == 0 ? 1 : 2)});
            }
            break;
        case 3:
            for (const auto& n : group.children) {
                expect(n,5,0);
                std::vector<Vector2> path;
                for (const auto& joint : n.children) {
                    expect(joint,6,8);
                    path.push_back(joint.payload.point(0));
                }
                if (!path.empty()) map.collisionPaths.push_back(std::move(path));
            }
            break;
        case 9:
            map.background=r.text(0,r.data.size());
            background=true;
            break;
        case 10:
            for (const auto& n : group.children) {
                expect(n,11,32);
                auto name=n.payload.text(0,24);
                if (name == "Player") continue;
                bool boss=name.starts_with("Boss") || name.starts_with("MBoss") ||
                    (name.size() >= 4 && name.substr(2,2)=="MB") ||
                    (name.size() >= 5 && name.substr(3,2)=="MB");
                map.enemies.push_back({name,n.payload.point(24),boss});
            }
            break;
        default: throw std::runtime_error("Unknown map group type");
        }
    }
    if (!bounds || !background) throw std::runtime_error("Missing map bounds/background");
    return map;
}
std::vector<ColVector> MapDefinition::collisionVectors() const {
    std::vector<ColVector> result;
    size_t count=0;
    for (const auto& path:collisionPaths) if (path.size()>1) count+=path.size()-1;
    result.reserve(count);
    for (const auto& path:collisionPaths) {
        size_t begin=result.size();
        for (size_t i=1;i<path.size();++i) result.emplace_back(path[i],path[i-1]);
        for (size_t i=begin;i<result.size();++i) {
            result[i].prev=i>begin ? &result[i-1] : nullptr;
            result[i].next=i+1<result.size() ? &result[i+1] : nullptr;
        }
    }
    return result;
}
GameData GameData::loadDirectory(const std::filesystem::path& directory) {
    GameData result;
    insert(result.weapons,parseWeapons(readFile(directory/"WeaponInfo.mch")));
    insert(result.parts,parseParts(readFile(directory/"PartsInfo.mch")));
    insert(result.machines,parseMachines(readFile(directory/"MachInfo.mch")));
    for (const auto& entry:std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension()==".map")
            result.maps.emplace(entry.path().stem().string(),parseMap(readFile(entry.path())));
    }
    return result;
}
}
