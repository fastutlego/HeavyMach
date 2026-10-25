#include "../Sources/Core/ItemBox.hpp"
#include <cassert>
#include <limits>
#include <vector>
using namespace heavymach;
namespace {
struct Host:ItemBoxHost {std::vector<int32_t> seq{0,0};size_t cursor=0;std::vector<ItemBoxEventKind> events;
 int32_t random31()override{assert(cursor<seq.size());return seq[cursor++];}
 void event(ItemBoxEventKind e)override{events.push_back(e);}};
}
void runItemBoxTests(){
 Host h;GobItemBox b(h);assert(h.cursor==2&&b.velocity.x==-100&&b.velocity.y==150);
 Host max;max.seq={2147483647,2147483647};GobItemBox hi(max);assert(hi.velocity.x==100&&hi.velocity.y==250); // float rounding retained.
 ItemBoxFrame f;f.hasPlayer=false;b.velocity={60,10};b.tick(f,h);assert(b.position.x==1&&b.position.y==0);assert(h.events.back()==ItemBoxEventKind::TickChildren);
 h.events.clear();f.motion.paused=true;b.tick(f,h);assert(b.position.x==1&&h.events.size()==1);
 b.show=false;h.events.clear();b.tick(f,h);assert(h.events.empty());b.show=true;f.motion.paused=false;
 GroundSegment ground;ground.collision=ColVector({100,0},{0,0});std::vector<GroundSegment> gs{ground};std::vector<int> ids{0};f.motion.ground=gs;f.motion.collisionCandidates=ids;
 b.position={50,1};b.velocity={0,-90};b.tick(f,h);assert(!b.landed&&b.velocity.y==30.0000019073486328125f&&b.position.y==.5f);
 b.position={50,.5f};b.velocity={0,-80};b.tick(f,h);assert(b.landed&&b.velocity.y==(-90.f*-.3f));
 // Landed survives arbitrarily many ticks, no expiry, no gravity integration.
 for(int i=0;i<5000;++i)b.tick(f,h);assert(!b.removed&&b.position.y==.5f);
 // Exact pickup boundary, y+12, deferred removal and base tick order.
 f.hasPlayer=true;f.playerPosition={70,-11.5f};h.events.clear();b.tick(f,h);assert(b.removed);assert(h.events.size()==3&&h.events[0]==ItemBoxEventKind::Pickup&&h.events[1]==ItemBoxEventKind::Remove&&h.events[2]==ItemBoxEventKind::TickChildren);
 Host h2;GobItemBox attract(h2);attract.landed=true;attract.position={0,0};f.motion.gameMode=1;f.playerPosition={100,100};h2.events.clear();attract.tick(f,h2);assert(attract.position.x==4&&attract.position.y==124.f/25.f);
 attract.inScreen=false;auto before=attract.position;attract.tick(f,h2);assert(attract.position.x==before.x&&attract.position.y==before.y);
 auto s=attract.draw();assert(s.tile=="ItemBox.png"&&s.frame==0&&s.cols==1&&s.rows==1&&s.z==2);
 assert(pointInObject({-20,0},{0,0},playerBounds()));assert(pointInObject({20,50},{0,0},playerBounds()));assert(!pointInObject({20.01f,50},{0,0},playerBounds()));
 assert(pointInObject({std::numeric_limits<float>::quiet_NaN(),10},{0,0},playerBounds()));
 auto pb=partsBounds(100,80,{2,4});assert(pb.left==-52&&pb.bottom==-44&&pb.right==48&&pb.top==36);
 ObjectBounds mb{};accumulateMachBounds(mb,{10,30},pb);assert(mb.left==-20&&mb.right==34&&mb.bottom==0&&mb.top==48);
 accumulateMachBounds(mb,{-100,0},pb);assert(mb.left==-126&&mb.right==34&&mb.top==48);
}
