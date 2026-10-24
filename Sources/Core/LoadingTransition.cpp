#include "LoadingTransition.hpp"
namespace heavymach {
namespace {
// Separate binary32 subtract/divide/add, as the ARM VFP helper calls.
void approach(float& value,float target,float divisor){float delta=target-value;delta=delta/divisor;value=value+delta;}
}
LoadingTransition::LoadingTransition(){
 const char* names[]={"Loading_Top.png","Loading_Bottom.png","Loading_Left.png","Loading_Right.png"};
 const Vector2 positions[]={{240,223},{240,100},{240,160},{240,160}};
 for(unsigned i=0;i<4;++i){sprites_[i].tile=names[i];sprites_[i].position=positions[i];sprites_[i].z=6;}
}
bool LoadingTransition::changeScreen(int screen){
 if(screen<1||screen>7)return false;
 screen_=screen;state_=0;return true;
}
void LoadingTransition::tick(const EventSink& sink){
 auto event=[&](LoadingEventKind kind){if(sink)sink({kind,screen_});};
 auto& top=sprites_[0].position;auto& bottom=sprites_[1].position;
 auto& left=sprites_[2].position;auto& right=sprites_[3].position;
 switch(state_){
 case 0:
  visible_=true;top={240,423};bottom={240,-100};left={-100,160};right={580,160};
  state_=1;event(LoadingEventKind::CloseSound);break;
 case 1:
  approach(top.y,223,6);approach(bottom.y,101,6);
  if(bottom.y>70){state_=2;event(LoadingEventKind::CloseSound);}break;
 case 2:
  approach(top.y,223,6);approach(bottom.y,101,6);
  approach(left.x,245,6);approach(right.x,235,6);
  if(left.x>=240){left.x=240;right.x=240;state_=3;}break;
 case 3:
  event(LoadingEventKind::RefreshScreen);state_=4;event(LoadingEventKind::OpenSound);break;
 case 4:
  approach(left.x,-110,10);approach(right.x,590,10);
  if(left.x<0){state_=5;event(LoadingEventKind::OpenSound);}break;
 case 5:
  approach(top.y,423,10);approach(bottom.y,-100,10);
  approach(left.x,-110,10);approach(right.x,590,10);
  if(top.y>422){visible_=false;state_=6;}break;
 case 6:break;
 }
}
}
