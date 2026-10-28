#include "HUDLayout.hpp"
#include "HUDConfiguration.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <limits>
using namespace heavymach::hud;
static bool near(double a,double b){return std::abs(a-b)<1e-9;}
static void equal(Rect a,Rect b){assert(near(a.x,b.x)&&near(a.y,b.y)&&near(a.width,b.width)&&near(a.height,b.height));}
template<class F> static void rejects(F f){bool rejected=false;try{f();}catch(const std::invalid_argument&){rejected=true;}assert(rejected);}
int main(){
    const Rect jump{0,0,68,68},weapon{67,0,72,62},menu{6,263,64,32};
    const Rect safe{24,8,648,304};
    const Transform original{{108,0},1};
    // The old renderer's exact centers, sizes and hit convention survive reset.
    for(auto r:{jump,weapon,menu,Rect{334,10,46,46},Rect{382,10,46,46},Rect{430,10,46,46}}){
        const auto g=resolve(r,original,safe);
        equal(g.hitRect(r),{108+r.x,r.y,r.width,r.height});
        assert(!g.hit(r,{108+r.x,r.y+r.height/2}));
        assert(g.hit(r,{108+r.x+r.width,r.y+r.height}));
    }
    // Original jump/weapon buttons overlap by one logical unit. Default mode
    // retains its existing receiver order; custom editing must flag this pair.
    assert(overlaps(jump,weapon));
    assert(!overlaps(jump,{68,0,72,62}));
    // Original buttons use binary32 comparisons; aim buttons use CGRect edges.
    assert(!jump.contains({1e-50,34}));
    auto aim=resolve({334,10,46,46},original,safe);
    assert(aim.hit({334,10,46,46},{442,10},HitEdges::standard));
    assert(!aim.hit({334,10,46,46},{488,56},HitEdges::standard));
    // A button outside the old central 480 region: artwork and hit area agree.
    const auto moved=resolve(jump,original,safe,Placement{1,1});
    equal(moved.visualBounds,{604,244,68,68});
    assert(moved.hit(jump,{638,278}));
    assert(!moved.hit(jump,{142,34}));
    // The weapon's icon, ammo and button receive one identical translation.
    auto w=resolve({72,0,128,64},original,safe,Placement{.6,.7});
    auto icon=w.transform.apply(Point{106,35});auto ammo=w.transform.apply(Point{120,26});
    assert(near(ammo.x-icon.x,14)&&near(ammo.y-icon.y,-9));
    assert(w.hit(weapon,w.transform.apply(Point{103,31})));
    // Touch -> local -> canvas round trips, including safe-area scaling.
    for(double scale:{.625,.8,1.})for(auto placement:{Placement{0,0},Placement{.27,.83},Placement{1,1}}){
        auto g=resolve(jump,{{52,7},scale},safe,placement);
        auto p=g.transform.apply(Point{34,34});auto q=g.transform.inverse(p);
        assert(near(q.x,34)&&near(q.y,34)&&g.hit(jump,p));
        auto saved=placementForOrigin(jump,{{52,7},scale},safe,{g.visualBounds.x,g.visualBounds.y});
        assert(near(saved.x,placement.x)&&near(saved.y,placement.y));
    }
    // Rotation changes safe insets; normalized placement stays right aligned.
    const Placement persisted{1,.25};
    for(auto area:{safe,Rect{0,8,648,304},Rect{12,12,1000,700}}){
        auto g=resolve(jump,original,area,persisted);
        assert(near(g.visualBounds.x+g.visualBounds.width,area.x+area.width));
    }
    const auto tiny=resolve(jump,original,{0,0,40,40},persisted);assert(!tiny.fits);
    auto restored=resolve(jump,original,safe,persisted);
    assert(restored.fits&&near(restored.visualBounds.x,604));
    // A drag beyond the safe region clamps the group, not just its pivot.
    const auto clamped=placementForOrigin(jump,original,safe,{-999,999});
    assert(clamped.x==0&&clamped.y==1);
    // Invalid persisted values cannot become invisible or unclickable controls.
    const double nan=std::numeric_limits<double>::quiet_NaN();
    rejects([&]{resolve(jump,original,safe,Placement{nan,0});});
    rejects([&]{resolve(jump,original,safe,Placement{1.1,0});});
    rejects([&]{resolve(jump,{{0,0},0},safe);});
    rejects([&]{resolve(jump,original,{0,0,-1,10});});
    rejects([&]{placementForOrigin(jump,original,safe,{0,nan});});
    Configuration config;config.legacy={{100,0},1};config.safe={0,0,680,320};
    assert(valid(config));config.positions[Group::weapon]=Placement{0,.5};assert(valid(config));
    auto target=config.group(Group::jump).visualBounds;config.positions[Group::weapon]=placementForOrigin(bounds[Group::weapon],config.legacy,config.safe,{target.x,target.y});assert(!valid(config));
    config.positions={};assert(valid(config));
    std::cout<<"HUD layout: original geometry, moved hits, groups, scaling, rotation, window recovery and invalid settings passed\n";
}
