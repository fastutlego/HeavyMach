#pragma once
#include <array>
#include <algorithm>
#include <cmath>
namespace heavymach {
enum class KeyAction : unsigned {left,right,aimLeft,aimRight,aimUp,aimDown,fire,jump,previousWeapon,nextWeapon,fine,confirm,back,focus,settings,nextTarget,count};
struct KeyboardInput {
 static constexpr unsigned count=unsigned(KeyAction::count);
 // USB HID usages describe physical keys independently of layout and transport.
 static constexpr std::array<unsigned,count> defaults={0x04,0x07,0x50,0x4f,0x52,0x51,0x09,0x2c,0x14,0x08,0xe1,0x28,0x29,0x2b,0x3b,0x15};
 std::array<unsigned,count> bindings=defaults;
 std::array<bool,256> keys{};
 float virtualTilt=0;
 bool set(unsigned key,bool down){if(key>=keys.size())return false;bool changed=keys[key]!=down;keys[key]=down;return changed;}
 bool down(KeyAction action)const{return keys[bindings[unsigned(action)]]||(action==KeyAction::fine&&bindings[unsigned(action)]==0xe1&&keys[0xe5]);}
 bool is(unsigned key,KeyAction action)const{return key==bindings[unsigned(action)];}
 bool handles(unsigned key)const{return key==0xe5||std::find(bindings.begin(),bindings.end(),key)!=bindings.end();}
 bool remap(KeyAction action,unsigned key){
  if(!key||key>=keys.size()||key==0xe3||key==0xe7)return false;
  auto i=unsigned(action);auto old=bindings[i];
  for(unsigned j=0;j<count;++j)if(j!=i&&bindings[j]==key)bindings[j]=old;
  bindings[i]=key;cancel();return true;
 }
 bool fine()const{return down(KeyAction::fine);}
 int direction()const{return int(down(KeyAction::right))-int(down(KeyAction::left));}
 bool moving()const{return down(KeyAction::left)||down(KeyAction::right);}
 bool aiming()const{return down(KeyAction::aimLeft)||down(KeyAction::aimRight)||down(KeyAction::aimUp)||down(KeyAction::aimDown);}
 float sample(){
  const int d=direction();if(!d){virtualTilt=0;return 0;}
  // Keep the original 0.03g dead zone and clamp(g*15) movement response.
  const float cap=fine()?.04f:1.f/15.f,step=fine()?.0005f:.003f;
  if(virtualTilt*d<=0)virtualTilt=float(d)*.0301f;
  virtualTilt=float(d)*std::min(cap,std::fabs(virtualTilt)+step);return virtualTilt;
 }
 void aim(float& x,float& y,float width,float height)const{
  float dx=float(int(down(KeyAction::aimRight))-int(down(KeyAction::aimLeft)));
  float dy=float(int(down(KeyAction::aimUp))-int(down(KeyAction::aimDown)));
  float speed=fine()?1.f:4.f;if(dx&&dy)speed*=.70710678118f;
  x=std::clamp(x+dx*speed,0.f,width);y=std::clamp(y+dy*speed,0.f,height);
 }
 void cancel(){keys.fill(false);virtualTilt=0;}
};
}
