#pragma once
#include "Combat.hpp"
#include <array>
#include <vector>
namespace heavymach {
struct TrailVertex {Vector2 position{},uv{};std::array<float,4> color{1,1,1,1};};
struct TrailPoint {Vector2 center{},normal{};float time=0;std::array<TrailVertex,2> vertices;};
struct TrailMesh {std::string tile;Vector2 translation{};std::vector<TrailVertex> vertices;int blend=0;unsigned sourceFactor=0x302,destinationFactor=0x303;bool setBlend=true;};
enum class TrailEventKind {Remove,TickChildren};
struct TrailHost {virtual ~TrailHost()=default;virtual int32_t random31()=0;virtual void event(TrailEventKind)=0;};
class GobTrailEffect {
public:
 Vector2 position{};std::string tile;int blend=0;
 bool wind=true,diffuse=true,removed=false;
 float maxS=1,maxT=1;
 GobTrailEffect()=default;
 explicit GobTrailEffect(const CombatEvent&);
 void configure(float width,float length,int blocks,float fadeDelay,float fadeTime);
 void addTrailPos(Vector2 worldPoint,double time,TrailHost&);
 void addValidPos(Vector2 localPoint,bool append,double time,TrailHost&);
 void tick(double time,bool paused,TrailHost&);
 TrailMesh draw()const;
 int lastPosition()const{return last_;}
 const std::vector<TrailPoint>& points()const{return points_;}
private:
 float width_=0,length_=0,fadeDelay_=0,fadeTime_=0;int blocks_=0,last_=0;bool odd_=false,initialized_=false;
 std::vector<TrailPoint> points_;
 void refreshVertices(int);
};
}
