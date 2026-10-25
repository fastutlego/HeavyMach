#include "Player.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <stdexcept>
namespace heavymach {
namespace {
constexpr LootDefinition loot[]={{1,100,"HP_Half"},{1,10,"HP_Max"},{1,4,"EXP_Qurter"},{1,2,"EXP_Half"},{1,1,"EXP_Max"},{1,50,"Defense"},{1,50,"Attack"},{1,50,"Booster"},{1,50,"Speed"},{0,50,"P_Wpn_Shotgun01"},{0,50,"P_Wpn_Canon01"},{0,50,"P_Wpn_Missile01"},{0,50,"P_Wpn_Shotgun02"},{0,50,"P_Wpn_Canon02"},{0,50,"P_Wpn_Missile02"},{0,50,"P_Wpn_Laser"},{0,50,"P_Wpn_Canon03"},{0,50,"P_Wpn_Missile03"},{0,50,"P_Wpn_RaderBomb"},{0,50,"P_Wpn_RaderMsl"},{0,50,"P_Wpn_RaderLsr"},{0,50,"P_Wpn_ElShock"}};
constexpr size_t grades[]={0x2f,0x2e,0x2d};
int32_t rnd(PlayerHost& h){auto n=h.random31();if(n<0)throw std::invalid_argument("random31 must be nonnegative");return n;}
void sound(PlayerHost& h,int n){h.event({.kind=PlayerEventKind::Sound,.index=n});}
int32_t addWrap(int32_t a,int32_t b){return std::bit_cast<int32_t>(uint32_t(a)+uint32_t(b));}
}
std::span<const LootDefinition> originalLootTable(){return loot;}
bool PlayerPassives::active(int i)const{return i>=0&&i<4&&slots[i].active;}
void PlayerPassives::add(std::string_view name,PlayerHost& h){
    int i=name=="Attack"?1:name=="Booster"?2:name=="Speed"?3:0;
    auto& s=slots[i];if(!s.active){s.active=true;s.time=0;}s.time=60.f+s.time;s.number=int32_t(s.time);
    h.event({.kind=PlayerEventKind::Passive,.index=i,.amount=s.number,.value=s.time,.active=true});
}
void PlayerPassives::tick(PlayerHost* h){
    for(int i=0;i<4;++i){auto& s=slots[i];if(!s.active)continue;
        s.time-=std::bit_cast<float>(0x3c888889u);auto n=int32_t(s.time);
        if(n!=s.number){s.number=n;if(h)h->event({.kind=PlayerEventKind::Passive,.index=i,.amount=n,.value=s.time,.active=true});}
        if(s.time<=0){s.active=false;s.time=0;if(h)h->event({.kind=PlayerEventKind::Passive,.index=i,.amount=s.number,.active=false});}
    }
}
GobHvM_Player::GobHvM_Player(SaveSlot& s,PlayerPassives* shared):passives(shared?*shared:ownedPassives_),slot_(s){player=true;machType=3;hp=s.i32(0x18);refreshMoveSpeed();}
void GobHvM_Player::setHPGauge(float fraction,PlayerHost& h)const{
    if(hasHPGauge)h.event({.kind=PlayerEventKind::HPGauge,.value=fraction,.position={-(fraction*-21.f+21.f),0}});
}
void GobHvM_Player::refreshMaxHp(PlayerHost& h){hp=GlobalInfo::maximumHP(slot_.bytes[0x2e]);slot_.setInt(0x1c,hp);slot_.setInt(0x18,hp);setHPGauge(1,h);}
void GobHvM_Player::refreshMoveSpeed(){moveSpeed=GlobalInfo::movementSpeed(slot_.bytes[0x2f]);}
void GobHvM_Player::openUpgrade(PlayerHost& h){
    upgradeStep=1;upgradeSelection=rnd(h)%3;int fallback=100;
    if(slot_.bytes[0x2f]<6)fallback=0;else if(upgradeSelection==0)upgradeSelection=1;
    if(slot_.bytes[0x2e]<6)fallback=1;else if(upgradeSelection==1)upgradeSelection=2;
    if(slot_.bytes[0x2d]>=6&&upgradeSelection==2)upgradeSelection=fallback;
    h.event({.kind=PlayerEventKind::UpgradeOpen,.index=upgradeSelection});
}
bool GobHvM_Player::selectUpgrade(int p,PlayerHost& h){
    if(p<0||p>2||slot_.bytes[grades[p]]>=6)return false;
    upgradeSelection=p;h.event({.kind=PlayerEventKind::UpgradeSelection,.index=p});sound(h,0);return true;
}
void GobHvM_Player::confirmUpgrade(PlayerHost& h){upgradeStep=2;upgradeParts(upgradeSelection,h);sound(h,1);h.event({.kind=PlayerEventKind::UpgradeClose});}
bool GobHvM_Player::upgradeParts(int p,PlayerHost& h){
    if(p<0||p>2||slot_.bytes[grades[p]]>5)return false;
    auto grade=++slot_.bytes[grades[p]];
    h.event({.kind=PlayerEventKind::Parts,.index=p,.amount=int32_t(grade)+1});
    if(p==0)refreshMoveSpeed();else if(p==1)refreshMaxHp(h);return true;
}
void GobHvM_Player::addExperience(int32_t amount,PlayerHost& h){
    GlobalInfo info(slot_);auto result=info.addExperience(amount);
    if(result.leveledUp){openUpgrade(h);sound(h,6);}
    h.event({.kind=PlayerEventKind::ExperienceGauge,.value=result.gauge});
}
void GobHvM_Player::pickupItem(const GameData& data,PlayerHost& h){
    int step=std::bit_cast<int16_t>(uint16_t(uint16_t(slot_.bytes[0x3b4])|uint16_t(slot_.bytes[0x3b5])<<8));
    if(step<0||step>22)throw std::invalid_argument("invalid original loot step");
    int index=-1,tile=0;
    if(step<22&&rnd(h)%50==0){index=step;slot_.bytes[0x3b4]=uint8_t(step+1);slot_.bytes[0x3b5]=0;}
    else {
        int total=0;for(int i=0;i<step;++i)total+=loot[i].weight;
        if(total==0)throw std::invalid_argument("original weighted loot has zero total");
        int r=rnd(h)%total;for(int i=0;i<step;++i){r-=loot[i].weight;if(r<0){index=i;tile=i;break;}}
    }
    if(index>=0){const auto& item=loot[index];std::string_view asset="ItemTile.png";bool found=true;
        if(item.type==0){auto it=data.weapons.find(std::string(item.name));if(it==data.weapons.end())found=false;
            else {GlobalInfo info(slot_);info.addWeaponItem(item.name,it->second.charge);h.event({.kind=PlayerEventKind::WeaponInventory});tile=it->second.uiTile;asset="WeaponTile.png";}}
        else if(item.name=="HP_Half"||item.name=="HP_Max"){
            auto maximum=slot_.i32(0x1c);hp=std::min(addWrap(hp,item.name=="HP_Half"?maximum/2:maximum),maximum);setHPGauge(float(hp)/float(maximum),h);
        }else if(item.name=="EXP_Qurter"||item.name=="EXP_Half"||item.name=="EXP_Max"){
            auto t=GlobalInfo::thresholds(slot_.level());auto interval=t.next-t.previous;
            addExperience(item.name=="EXP_Qurter"?interval/4:item.name=="EXP_Half"?interval/2:int32_t(float(interval)/1.5f),h);
        }else passives.add(item.name,h);
        if(found&&h.acquireParticle())h.event({.kind=PlayerEventKind::PickupParticle,.index=tile,.life=2,.scaleVelocity=.5f,.position={position.x,position.y+16},.velocity={0,30},.name=asset});
    }
    sound(h,3);
}
void GobHvM_Player::onDamage(int32_t damage,PlayerHost& h){
    hp=std::bit_cast<int32_t>(uint32_t(hp)-uint32_t(damage));
    if(hp<1){hp=0;sound(h,14);h.event({.kind=PlayerEventKind::GameOver});}
    sound(h,8+rnd(h)%3);setHPGauge(float(hp)/float(slot_.i32(0x1c)),h);
    if(float(damage)>10)h.event({.kind=PlayerEventKind::Quake,.amount=4,.value=std::max(float(damage)/5.f,10.f),.delay=.3f});
}
void GobHvM_Player::setTap(Vector2 touch,int state,Vector2 screen,Vector2 rot,WeaponState* weapon){
    if(!weapon||hp<1)return;
    float dx=touch.x-(screen.x+rot.x),dy=touch.y-(screen.y+rot.y);
    if(std::fabs(dx)<100&&std::fabs(dy)<60&&state==0){if(dashVel==0&&slot_.bytes[0x2f]>2)dashVel=dx<0?-5.f:5.f;}
    else if(state==2){setWeaponFire(*weapon,false,{position.x+dx,position.y+dy});return;}
    if(!(dx>0))return;
    if(!(std::fabs(dx)>100)&&!(std::fabs(dy)>100))return;
    setWeaponAim(*weapon,float(std::atan2(double(dy),double(dx))));setWeaponFire(*weapon,state!=2,{position.x+dx,position.y+dy});
}
}
