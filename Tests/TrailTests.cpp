#include "../Sources/Core/Trail.hpp"
#include <cassert>
#include <cmath>
#include <vector>
using namespace heavymach;
namespace {struct Host:TrailHost{std::vector<int32_t> seq;size_t cursor=0;std::vector<TrailEventKind> events;int32_t random31()override{assert(cursor<seq.size());return seq[cursor++];}void event(TrailEventKind e)override{events.push_back(e);}};}
void runTrailTests(){
 Host h;GobTrailEffect t;t.configure(4,10,4,.1f,.8f);t.wind=false;t.diffuse=false;t.position={100,20};t.tile="trail";
 t.addTrailPos({100,20},0,h);assert(t.lastPosition()==1&&t.draw().vertices.empty());assert(std::isnan(t.points()[1].normal.x));
 t.addTrailPos({125,20},0,h);assert(t.lastPosition()==3&&h.cursor==0);auto mesh=t.draw();assert(mesh.vertices.size()==8&&mesh.translation.x==100);
 assert(t.points()[0].center.x==0&&t.points()[1].center.x==10&&t.points()[2].center.x==20&&t.points()[3].center.x==25);
 assert(mesh.vertices[0].position.y==-2&&mesh.vertices[1].position.y==2);assert(mesh.vertices[0].uv.y==1&&mesh.vertices[2].uv.y==0&&mesh.vertices[4].uv.y==1);
 t.tick(.5,false,h);assert(t.points()[0].vertices[0].color[3]==0);assert(std::fabs(t.points()[1].vertices[0].color[3]-.5f)<.000001f);
 h.events.clear();t.tick(1,true,h);assert(!t.removed&&h.events.size()==1);t.tick(1,false,h);assert(t.removed&&h.events[h.events.size()-2]==TrailEventKind::Remove);
 // Exact positive cross test is directional; only the clockwise corner consumes RNG.
 GobTrailEffect curve;curve.configure(4,10,4,1,1);h.seq={1073741824};h.cursor=0;
 curve.addValidPos({0,0},true,0,h);curve.addValidPos({10,0},true,0,h);curve.addValidPos({10,-10},true,0,h);assert(h.cursor==1);
 auto n=curve.points()[1].normal;assert(n.x<0&&n.y<0&&std::fabs(std::sqrt(n.x*n.x+n.y*n.y)-3.5f)<.00001f);
 // Capacity shift preserves order and alternating UV.
 GobTrailEffect capped;capped.configure(4,10,2,1,1);capped.wind=capped.diffuse=false;
 capped.addTrailPos({0,0},0,h);capped.addTrailPos({35,0},0,h);assert(capped.lastPosition()==2&&capped.points()[0].center.x==20&&capped.points()[1].center.x==30&&capped.points()[2].center.x==35);
 // Active rows consume diffuse then wind draws; pause/fully-faded consume none.
 GobTrailEffect effect;effect.configure(4,10,4,1,1);effect.addTrailPos({0,0},0,h);effect.addTrailPos({15,0},0,h);
 h.seq=std::vector<int32_t>(6,1073741824);h.cursor=0;effect.tick(.1,false,h);assert(h.cursor==6);
 assert(effect.points()[1].vertices[0].position.y==-2.05f&&effect.points()[1].vertices[1].position.y==2.05f);
 effect.tick(.2,true,h);assert(h.cursor==6);effect.tick(3,false,h);assert(h.cursor==6&&effect.removed);
 effect.blend=2;assert(effect.draw().sourceFactor==0x302&&effect.draw().destinationFactor==1);
}
