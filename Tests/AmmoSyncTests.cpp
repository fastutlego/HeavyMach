// Regression for the internal SaveSlot/charge bridge. This fixture exercises
// pickupItem and the burst-completion event without modifying shipped maps.
#include "World.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "ItemBox.hpp"
#include "Trail.hpp"
#include <variant>
#include <memory>
#define private public
#include "GameSession.hpp"
#undef private
#include "../Sources/Core/GameSession.cpp"
#include <iostream>
#include <cassert>
using namespace heavymach;
int main(){
 auto data=GameData::loadDirectory(std::filesystem::path(__FILE__).parent_path().parent_path()/"Resources/RawData");
 // Only original nextStage/gameOver request persistence; ordinary writes
 // remain memory updates (saveDataFile callers0xb8f0/0xbf74).
 SaveSlot persistenceSlot;GlobalInfo(persistenceSlot).selectSlot(1);
 GameSession persistence(data,persistenceSlot,[](std::string_view){return Vector2{128,128};},[]{return 768;});
 persistence.start(0,0,0);persistence.takeEvents();
 persistence.impl_->player->hp=123;
 persistence.impl_->changed();
 assert(persistenceSlot.i32(0x18)==256); // Only refreshMaxHp writes slotHP in original.
 for(const auto& e:persistence.takeEvents())assert(e.kind!=SessionEventKind::PersistSave);
 auto countPersistence=[&]{int count=0;for(const auto& e:persistence.takeEvents())if(e.kind==SessionEventKind::PersistSave){++count;assert(e.savedSlot&&e.savedSlot->bytes==persistenceSlot.bytes);}return count;};
 persistence.impl_->event(WorldEvent{WorldEventKind::SaveProgress,0,{0,1}});
 assert(countPersistence()==1);
 persistence.impl_->event(PlayerEvent{.kind=PlayerEventKind::GameOver});
 assert(countPersistence()==1);
 persistence.impl_->event(WorldEvent{WorldEventKind::SaveProgress,0,{0,2}});
 auto savedAtCall=persistenceSlot.bytes;
 persistenceSlot.setInt(0x20,12345); // Later in the same tick, before host drains queue.
 bool sawSnapshot=false;
 for(const auto& e:persistence.takeEvents())if(e.kind==SessionEventKind::PersistSave){sawSnapshot=true;assert(e.savedSlot&&e.savedSlot->bytes==savedAtCall);}
 assert(sawSnapshot&&persistenceSlot.experience()==12345);
 // GuiGame::changeWeapon0x178f0 writes 99 to selected default inventory.
 SaveSlot defaultSlot;GlobalInfo(defaultSlot).selectSlot(1);
 defaultSlot.setInt(0x4c,7);
 GameSession defaults(data,defaultSlot,[](std::string_view){return Vector2{128,128};},[]{return 768;});
 defaults.start(0,0,0);assert(defaultSlot.weaponCharge(0)==99);
 assert(defaults.impl_->playerGraph.parts[2]->weapon.aimDir==0);
 assert(defaults.sprites().empty()&&defaults.renderItems().empty());
 defaults.selectWeapon(1);defaultSlot.setInt(0x4c,8);defaults.selectWeapon(0);
 assert(defaultSlot.weaponCharge(0)==99&&defaults.impl_->charge==99);
 // Original machine draw reuses its layer until a part is replaced.
 defaults.impl_->collect();auto& graph=defaults.impl_->playerGraph;
 assert(!graph.drawOrderDirty&&graph.drawOrder.size()==3);
 const auto* orderStorage=graph.drawOrder.data();auto* oldGun=graph.drawOrder[2];
 for(int n=0;n<10;++n){defaults.impl_->collect();assert(graph.drawOrder.data()==orderStorage&&!graph.drawOrderDirty);}
 defaults.selectWeapon(1);assert(graph.drawOrderDirty);
 defaults.impl_->collect();assert(!graph.drawOrderDirty&&graph.drawOrder[2]==graph.parts[2].get()&&graph.drawOrder[2]!=oldGun);
 defaults.selectWeapon(0);
 defaultSlot.setInt(0x4c,9);
 defaults.impl_->event(PlayerEvent{.kind=PlayerEventKind::WeaponInventory});
 assert(defaultSlot.weaponCharge(0)==99&&defaults.impl_->charge==99);
 SaveSlot slot;GlobalInfo(slot).selectSlot(1);
 // Force original weighted loot index18 (RaderBomb), without changing resources.
 slot.bytes[0x3b4]=22;slot.bytes[0x3b5]=0;
 GameSession game(data,slot,[](std::string_view){return Vector2{128,128};},[]{return 768;});
 game.start(0,0,0);game.selectWeapon(1);
 auto& impl=*game.impl_;int before=slot.weaponCharge(1);
 impl.player->pickupItem(data,impl);
 int picked=slot.weaponCharge(1),cached=impl.charge;
 slot.bytes[0x34+0x1c+23]=0x5a; // Original decrements charge only, retaining name padding.
 --impl.charge;impl.event(CombatEvent{CombatEventKind::ChargeChanged});
 assert(slot.bytes[0x34+0x1c+23]==0x5a);
 std::cout<<"ammo before="<<before<<" pickup="<<picked<<" cached="<<cached<<" after_one_burst="<<slot.weaponCharge(1)<<"\n";
 assert(picked>before);assert(cached==picked);assert(slot.weaponCharge(1)==picked-1);
 // Original 0x12800..1282a calls removeCurWeapon synchronously. Its
 // 0x1c556..1c574 inventory deletion/default selection precedes item tick.
 SaveData save;auto& exhausted=save.slots[0];GlobalInfo(exhausted).selectSlot(1);
 exhausted.bytes[0x3b4]=22;exhausted.setWeapon(1,"P_Wpn_RaderBomb",1);
 GameSession last(data,exhausted,[](std::string_view){return Vector2{128,128};},[]{return 768;});
 last.start(0,0,0);last.selectWeapon(1);
 auto& host=*last.impl_;host.enemies.clear();host.dynamic.clear();
 host.player->position={240,100};host.player->machState=0;host.player->speed=0;
 auto& weapon=host.playerGraph.parts[2]->weapon;
 weapon.fireTime=-1;weapon.fireDelay=0;weapon.fireCount=1;weapon.isFire=false;
 GobItemBox box(host);box.position={240,88};box.landed=true;box.inScreen=true;
 host.dynamic.emplace_back(box);last.takeEvents();
 last.tick(1.0/60.0,0);
 int failures=0;
 auto check=[&](bool ok,const char* name){std::cout<<(ok?"PASS ":"FAIL ")<<name<<"\n";if(!ok)++failures;};
 check(host.selected==0,"exhaustion selects default weapon");
 check(exhausted.weaponCount()==4,"same-frame reacquisition retains four inventory entries");
 int bomb=-1;for(int n=0;n<exhausted.weaponCount();++n)if(exhausted.weaponName(n)=="P_Wpn_RaderBomb")bomb=n;
 check(bomb==3,"reacquired RaderBomb appends after surviving weapons");
 check(bomb>=0&&exhausted.weaponCharge(size_t(bomb))==5,"reacquired charge is original pickup charge five");
 check(exhausted.weaponName(1)=="P_Wpn_RaderMsl"&&exhausted.weaponName(2)=="P_Wpn_RaderLsr","surviving inventory order");
 auto decoded=SaveData::decode(save.encode());
 check(decoded.slots[0].bytes==exhausted.bytes,"save encode/decode preserves resulting bytes");
 check(decoded.slots[0].weaponCount()==4&&decoded.slots[0].weaponName(3)=="P_Wpn_RaderBomb"&&decoded.slots[0].weaponCharge(3)==5,"persisted reacquisition survives reload");
 auto events=last.takeEvents();int discardSounds=0;bool saved=false;
 for(const auto& e:events){if(e.kind==SessionEventKind::AudioIndex&&e.index==5)++discardSounds;if(e.kind==SessionEventKind::SaveChanged)saved=true;}
 check(saved,"session reports in-memory save change");check(discardSounds==1,"original discard sound five exactly once");
 // 0x1c408 particle: removed weapon tile0, life2, pos player+(0,40),
 // scale1, rotation velocity15, gravity250, velocity(-50,150), blend0.
 const ParticleState* debris=nullptr;
 for(const auto& p:host.world.particles)if(p.show&&p.sprite.tile=="P_Wpn_RaderBomb.png")debris=&p;
 check(debris!=nullptr,"exhaustion emits original discarded weapon particle");
 if(debris){check(debris->liveTime==2&&debris->sprite.frame==0&&debris->sprite.blend==0&&debris->sprite.scale.x==1&&debris->sprite.scale.y==1,"discard particle visual contract");
 check(debris->sprite.position.x==240&&debris->sprite.position.y==140&&debris->velocity.x==-50&&debris->velocity.y==150&&debris->gravity==250&&debris->rotationVelocity==15,"discard particle initial motion contract");}
 std::cout<<"weapon-exhaustion failures="<<failures<<"\n";return failures?1:0;
}
