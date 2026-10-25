#include "../Sources/Core/GameData.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace {
std::vector<uint8_t> bytes(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    assert(input);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
void parse(const std::filesystem::path& path, std::span<const uint8_t> data) {
    using heavymach::GameData;
    if (path.filename()=="WeaponInfo.mch") (void)GameData::parseWeapons(data);
    else if (path.filename()=="PartsInfo.mch") (void)GameData::parseParts(data);
    else if (path.filename()=="MachInfo.mch") (void)GameData::parseMachines(data);
    else (void)GameData::parseMap(data);
}
void rejects(const std::filesystem::path& path, std::span<const uint8_t> data) {
    bool rejected=false;
    try { parse(path,data); } catch (const std::runtime_error&) { rejected=true; }
    assert(rejected);
}
}
void runGameDataTests() {
    using namespace heavymach;
    const auto directory=std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources/RawData";
    auto data=GameData::loadDirectory(directory);
    assert(data.weapons.size()==160 && data.parts.size()==294);
    assert(data.machines.size()==720 && data.maps.size()==43);
    const auto& gun=data.weapons.at("P_Wpn_Gun01");
    assert(gun.dmg==10 && gun.shotCnt==1 && gun.charge==99);
    assert(gun.spd==1200 && std::abs(gun.delay-0.12f)<0.000001f);
    assert(gun.effectTypeNumber()==1 && gun.fireSFX=="Wpn_Gun01");
    const auto& body=data.parts.at("P_Body_01");
    assert(body.tileCnt==2 && body.sockets.size()==2);
    assert(body.sockets[0].x==-1 && body.sockets[0].y==-12);
    assert(body.tileName()=="P_Body_01.png");
    assert(data.machines.at("Player").hpMax==512);
    assert(data.machines.at("E0501").giveExp==30);
    assert(!data.machines.at("Player").parts.empty());
    const auto& map=data.maps.at("Stage01");
    assert(map.left==-1060 && map.top==468 && map.right==2664 && map.bottom==-160);
    for (const auto& [name,m] : data.maps) {
        for (const auto& enemy:m.enemies) assert(enemy.name!="Player");
        for (const auto& object:m.objects) assert(object.layer==1 || object.layer==2);
        auto segments=m.collisionVectors();
        size_t at=0;
        for (const auto& path:m.collisionPaths) {
            for (size_t i=1;i<path.size();++i,++at) {
                const auto& line=segments[at];
                assert(line.origin.x==path[i].x && line.origin.y==path[i].y);
                assert(line.prev==(i==1 ? nullptr : &segments[at-1]));
                assert(line.next==(i+1==path.size() ? nullptr : &segments[at+1]));
            }
        }
        assert(at==segments.size());
    }
    size_t files=0;
    for (const auto& entry:std::filesystem::directory_iterator(directory)) {
        auto raw=bytes(entry.path());
        parse(entry.path(),raw);
        for (size_t cut : {size_t(0),size_t(4),size_t(12),raw.size()/2,raw.size()-1})
            rejects(entry.path(),std::span(raw).first(cut));
        raw.push_back(0);
        rejects(entry.path(),raw);
        raw.pop_back();
        // Corrupt the root payload length; bounds must fail before allocation.
        for (size_t i=8;i<12;++i) raw[i]=255;
        rejects(entry.path(),raw);
        ++files;
    }
    assert(files==46);
}
