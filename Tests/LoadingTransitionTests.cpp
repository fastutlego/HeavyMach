#include "../Sources/Core/LoadingTransition.hpp"
#include <cassert>
#include <cmath>
#include <vector>
using namespace heavymach;
void runLoadingTransitionTests(){
 LoadingTransition t;std::vector<LoadingEventKind> events;int refreshes=0;
 auto receive=[&](LoadingEvent e){events.push_back(e.kind);assert(e.screen==5);if(e.kind==LoadingEventKind::RefreshScreen){++refreshes;assert(t.state()==3);assert(t.visible());assert(t.panels()[2].position.x==240);assert(t.panels()[3].position.x==240);}};
 assert(t.state()==6&&!t.visible()&&t.screen()==1&&t.sprites().empty());
 t.tick(receive);assert(events.empty());
 assert(!t.changeScreen(0)&&!t.changeScreen(8)&&!t.changeScreen(-1));assert(t.state()==6);
 assert(t.changeScreen(5));assert(t.state()==0&&!t.visible());t.tick(receive);
 assert(t.state()==1&&t.visible()&&t.sprites().size()==4);
 assert(t.panels()[0].position.y==423&&t.panels()[1].position.y==-100);
 assert(t.panels()[2].position.x==-100&&t.panels()[3].position.x==580);
 t.tick(receive);assert(std::fabs(t.panels()[0].position.y-389.66666f)<.0001f);
 assert(t.panels()[1].position.y==-66.5f&&t.panels()[2].position.x==-100);
 int ticks=2;int enters[7]={0,1,0,0,0,0,0};
 while(t.state()!=6&&ticks<200){int old=t.state();t.tick(receive);++ticks;if(t.state()!=old)enters[t.state()]=ticks;}
 assert(refreshes==1&&ticks<200);
 assert(events==std::vector<LoadingEventKind>({LoadingEventKind::CloseSound,LoadingEventKind::CloseSound,LoadingEventKind::RefreshScreen,LoadingEventKind::OpenSound,LoadingEventKind::OpenSound}));
 // Measured integer boundaries from binary32 recurrence; no wall-time constants.
 assert(enters[2]==12&&enters[3]==36&&enters[4]==37&&enters[5]==48&&enters[6]==99);
 assert(t.sprites().empty());t.tick(receive);assert(refreshes==1);
 assert(t.changeScreen(2));t.tick({});t.tick({});assert(t.changeScreen(7));
 // changeScreen does not hide/reposition until the next tick.
 assert(t.visible()&&t.state()==0&&t.panels()[0].position.y<423);
 t.tick({});assert(t.screen()==7&&t.panels()[0].position.y==423);
 for(const auto& p:t.sprites()){assert(p.z==6&&p.blend==0&&p.frame==0&&p.cols==1&&p.rows==1&&p.alpha==1&&p.scale.x==1);}
 // Expanded presentation does not change panel recurrence or refresh/sound timing.
 for(int step=0;step<100;++step){
  auto native=t.expandedPanels(480,320),wide=t.expandedPanels(960,320),tall=t.expandedPanels(480,360);
  for(size_t i=0;i<4;++i){
   assert(native[i].sprite.position.x==t.panels()[i].position.x&&native[i].sprite.position.y==t.panels()[i].position.y);
   assert(wide[i].left==240&&wide[i].right==240&&wide[i].sprite.scale.x==1);
   assert(tall[i].left==0&&tall[i].right==0&&tall[i].sprite.scale.y==1);
  }
  assert(tall[0].top==20&&tall[0].bottom==0&&tall[1].bottom==20&&tall[1].top==0);
  assert(tall[2].top==20&&tall[2].bottom==20);
  if(t.state()==3||t.state()==4){assert(wide[0].sprite.position.y==223&&wide[1].sprite.position.y==101);}
  if(t.state()==3){assert(wide[2].sprite.position.x==480&&wide[3].sprite.position.x==480);}
  t.tick({});
 }
}
#ifdef HEAVYMACH_LOADING_TEST_MAIN
int main(){runLoadingTransitionTests();}
#endif
