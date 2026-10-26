#include "KeyboardInput.hpp"
#include "CanvasGeometry.hpp"
#include "Viewport.hpp"
#include <cassert>
#include <cmath>
using namespace heavymach;
int main(){
 KeyboardInput k;assert(!k.moving());assert(k.set(7,true));assert(!k.set(7,true));
 for(int n=0;n<100;++n)k.sample();assert(std::fabs(k.virtualTilt-1.f/15.f)<1e-6);
 k.set(4,true);assert(k.sample()==0);k.set(7,false);assert(k.sample()<-.03f);
 k.set(4,false);assert(!k.moving());assert(k.sample()==0);
 k.set(0xe1,true);k.set(7,true);for(int n=0;n<100;++n)k.sample();assert(k.virtualTilt==.04f);
 k.cancel();assert(!k.moving());assert(!k.fine());
 assert(k.remap(KeyAction::fire,7));assert(k.bindings[unsigned(KeyAction::right)]==9);k.set(7,true);assert(k.down(KeyAction::fire));assert(!k.moving());
 assert(!k.remap(KeyAction::fire,0xe3));k.cancel();assert(!k.down(KeyAction::fire));
 k=KeyboardInput{};k.set(0x4f,true);k.set(0x52,true);float x=479,y=359;k.aim(x,y,480,360);assert(x==480&&y==360);
 auto c=CanvasGeometry::forDrawable(1024,768,true);assert(c.width==480&&c.height==360);
 auto v=Viewport::drawable(2048,1536,2,c.width,c.height);assert(v.x==0&&v.y==0);auto p=v.logical(1024,0);assert(p&&p->first==480&&p->second==360);
 auto wide=CanvasGeometry::forDrawable(1920,1080,true);assert(wide.height==320&&wide.width>568);
}
