#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
#include <stdexcept>

namespace heavymach::hud {
// Logical canvas coordinates: x right, y up. No world/camera coordinates here.
struct Point { double x=0,y=0; };
enum class HitEdges { original, standard };
struct Rect {
    double x=0,y=0,width=0,height=0;
    bool contains(Point p,HitEdges edges=HitEdges::original) const {
        if(edges==HitEdges::standard)return p.x>=x&&p.x<x+width&&p.y>=y&&p.y<y+height;
        // Preserve the original HUD button boundary convention.
        return float(p.x)>float(x)&&float(p.x)<=float(x+width)&&
               float(p.y)>float(y)&&float(p.y)<=float(y+height);
    }
};
struct Transform {
    Point offset;
    double scale=1;
    Point apply(Point p) const {return {offset.x+p.x*scale,offset.y+p.y*scale};}
    Point inverse(Point p) const {return {(p.x-offset.x)/scale,(p.y-offset.y)/scale};}
    Rect apply(Rect r) const {auto p=apply(Point{r.x,r.y});return {p.x,p.y,r.width*scale,r.height*scale};}
};
// Position within the available travel of a complete group, not its button alone.
// 0/1 pins its visual edge to the safe rectangle's left/right or bottom/top.
struct Placement {double x=0,y=0;};
struct ResolvedGroup {
    Transform transform;
    Rect visualBounds;
    bool fits=true;
    Rect hitRect(Rect originalHitRect) const {return transform.apply(originalHitRect);}
    bool hit(Rect originalHitRect,Point canvasPoint,HitEdges edges=HitEdges::original) const {
        return originalHitRect.contains(transform.inverse(canvasPoint),edges);
    }
};
inline bool finite(Rect r) {
    return std::isfinite(r.x)&&std::isfinite(r.y)&&std::isfinite(r.width)&&std::isfinite(r.height)&&r.width>0&&r.height>0;
}
inline void validate(Rect localBounds,Transform legacy,Rect safe) {
    if(!finite(localBounds)||!finite(safe)||!std::isfinite(legacy.scale)||legacy.scale<=0||
       !std::isfinite(legacy.offset.x)||!std::isfinite(legacy.offset.y))
        throw std::invalid_argument("Invalid HUD geometry");
}
inline ResolvedGroup resolve(Rect localBounds,Transform legacy,Rect safe,std::optional<Placement> placement={}) {
    validate(localBounds,legacy,safe);
    auto bounds=legacy.apply(localBounds);
    if(!placement)return {legacy,bounds,true}; // Default artwork, including its original edge decoration.
    if(!std::isfinite(placement->x)||!std::isfinite(placement->y)||placement->x<0||placement->x>1||placement->y<0||placement->y>1)
        throw std::invalid_argument("HUD placement outside [0, 1]");
    const double travelX=safe.width-bounds.width,travelY=safe.height-bounds.height;
    // A small window may not fit a group. Center it temporarily and report that
    // condition; never silently change the saved placement or group scale.
    const double x=safe.x+(travelX<0?travelX/2:travelX*placement->x);
    const double y=safe.y+(travelY<0?travelY/2:travelY*placement->y);
    legacy.offset.x+=x-bounds.x;legacy.offset.y+=y-bounds.y;
    return {legacy,{x,y,bounds.width,bounds.height},travelX>=0&&travelY>=0};
}
// An in-memory drag draft. The caller persists a validated layout on Apply only.
inline Placement placementForOrigin(Rect localBounds,Transform legacy,Rect safe,Point wantedOrigin) {
    validate(localBounds,legacy,safe);
    if(!std::isfinite(wantedOrigin.x)||!std::isfinite(wantedOrigin.y))throw std::invalid_argument("Invalid HUD drag");
    const auto bounds=legacy.apply(localBounds);
    auto component=[](double p,double start,double travel){return travel>0?std::clamp((p-start)/travel,0.,1.):.5;};
    return {component(wantedOrigin.x,safe.x,safe.width-bounds.width),component(wantedOrigin.y,safe.y,safe.height-bounds.height)};
}
inline bool overlaps(Rect a,Rect b) {
    return std::min(a.x+a.width,b.x+b.width)>std::max(a.x,b.x)&&
           std::min(a.y+a.height,b.y+b.height)>std::max(a.y,b.y);
}
} // namespace heavymach::hud
