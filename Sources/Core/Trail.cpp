#include "Trail.hpp"
#include <bit>
#include <cmath>
#include <stdexcept>
namespace heavymach {
namespace {float randomUnit(TrailHost& h){auto n=h.random31();if(n<0)throw std::invalid_argument("random31");return float(n)*std::bit_cast<float>(0x30000000u);}float len(Vector2 v){return std::sqrt(v.x*v.x+v.y*v.y);} }
GobTrailEffect::GobTrailEffect(const CombatEvent& e):position(e.pos),tile(e.tile),blend(e.blend),wind(e.wind),diffuse(e.diffuse){configure(e.width,e.length,e.blocks,e.fadeDelay,e.fadeTime);}
void GobTrailEffect::configure(float w,float l,int blocks,float delay,float fade){
 if(blocks<1||!(l>0)||!std::isfinite(l))throw std::invalid_argument("invalid trail capacity/length");
 width_=w;length_=l;blocks_=blocks;fadeDelay_=delay;fadeTime_=fade;last_=0;odd_=false;initialized_=false;points_.assign(size_t(blocks)+2,{});
}
void GobTrailEffect::refreshVertices(int i){auto& p=points_[i];p.vertices[0].position={p.center.x+p.normal.x,p.center.y+p.normal.y};p.vertices[1].position={p.center.x-p.normal.x,p.center.y-p.normal.y};}
void GobTrailEffect::addValidPos(Vector2 p,bool append,double time,TrailHost& h){
 if(points_.empty())throw std::logic_error("trail not configured");
 if(last_>blocks_){for(int i=0;i<blocks_;++i)points_[i]=points_[i+1];--last_;}
 auto& q=points_[last_];q.center=p;q.time=float(time);q.vertices[0].uv={0,odd_?0:maxT};q.vertices[1].uv={maxS,odd_?0:maxT};
 if(last_<1)q.normal={0,0};else {
  const auto prev=points_[last_-1].center;Vector2 n{p.y-prev.y,-(p.x-prev.x)};float d=len(n);q.normal={(n.x/d)*width_*.5f,(n.y/d)*width_*.5f};
  if(!append||last_<2){points_[0].normal=q.normal;refreshVertices(0);}
  else {
   auto& middle=points_[last_-1];auto before=points_[last_-2].center;
   Vector2 a{before.x-middle.center.x,before.y-middle.center.y},b{p.x-middle.center.x,p.y-middle.center.y};
   float da=len(a),db=len(b);a={(a.x/da)*width_,(a.y/da)*width_};b={width_*(b.x/db),width_*(b.y/db)};
   if(a.x*b.y-a.y*b.x>.1f){float w=width_+randomUnit(h)*6.f;Vector2 sum{a.x+b.x,a.y+b.y};float ds=len(sum);middle.normal={w*(sum.x/ds)*.5f,w*(sum.y/ds)*.5f};refreshVertices(last_-1);}
  }
 }
 refreshVertices(last_);q.vertices[0].color={1,1,1,1};q.vertices[1].color={1,1,1,1};initialized_=true;
 if(append){++last_;odd_=!odd_;}
}
void GobTrailEffect::addTrailPos(Vector2 world,double time,TrailHost& h){
 Vector2 p{world.x-position.x,world.y-position.y};
 if(last_<1)addValidPos(p,true,time,h);
 else {auto prev=points_[last_-1].center;Vector2 delta{p.x-prev.x,p.y-prev.y};float sq=delta.x*delta.x+delta.y*delta.y;
  if(sq>=length_*length_){float count=float(std::sqrt(double(sq))/double(length_));if(!std::isfinite(count))throw std::invalid_argument("nonfinite trail interpolation");delta.x/=count;delta.y/=count;
   while(count>=1){prev.x+=delta.x;prev.y+=delta.y;addValidPos(prev,true,time,h);float next=count-1.f;if(next==count)throw std::overflow_error("trail interpolation nonprogress");count=next;}
  }
 }
 addValidPos(p,false,time,h);
}
void GobTrailEffect::tick(double time,bool paused,TrailHost& h){
 if(!paused&&initialized_){bool alive=false;
  for(int i=0;i<=last_;++i){auto& p=points_[i];float elapsed=float(time-double(p.time));bool active=true;
   if(elapsed>fadeDelay_){elapsed-=fadeDelay_;float alpha=elapsed<fadeTime_?1.f-elapsed/fadeTime_:0.f;if(i==0)alpha=0;
    p.vertices[0].color[3]=alpha;p.vertices[1].color[3]=alpha;active=alpha!=0;
   }
   if(!active)continue;
   if(diffuse){float amount=randomUnit(h)*.05f;auto& a=p.vertices[0].position;auto& b=p.vertices[1].position;
    a.x=amount*p.normal.x+a.x;a.y=amount*p.normal.y+a.y;b.x=b.x-amount*p.normal.x;b.y=b.y-amount*p.normal.y;}
   if(wind){float amount=std::sin(30.f*p.time)/(randomUnit(h)*10.f+20.f);auto& a=p.vertices[0].position;auto& b=p.vertices[1].position;
    a.x=amount*p.normal.x+a.x;a.y=amount*p.normal.y+a.y;b.x=amount*p.normal.x+b.x;b.y=amount*p.normal.y+b.y;}
   alive=true;
  }
  if(last_>0&&!alive){removed=true;h.event(TrailEventKind::Remove);}
 }
 h.event(TrailEventKind::TickChildren);
}
TrailMesh GobTrailEffect::draw()const{
 TrailMesh m;m.tile=tile;m.translation=position;m.blend=blend;
 if(blend==1)m.sourceFactor=0x300;else if(blend==2||blend==3)m.destinationFactor=1;else if(blend!=0)m.setBlend=false;
 if(last_>1)for(int i=0;i<=last_;++i){m.vertices.push_back(points_[i].vertices[0]);m.vertices.push_back(points_[i].vertices[1]);}return m;
}
}
