#include "../Sources/Core/Geometry.hpp"
#include <cassert>
#include <bit>
#include <cmath>
#include <cstdint>
#include <stdexcept>
using namespace heavymach;
namespace {
bool near(float a, float b) { return std::fabs(a-b) < 0.00001f; }
void pointIs(Vector2 v, float x, float y) { assert(near(v.x,x) && near(v.y,y)); }
}
void runGeometryTests() {
    Vector2 a{3,4}; assert(a.length()==5 && a.squereLength()==25);
    assert(a.dotProduct({2,-1})==2 && a.crossProduct({2,-1})==-11);
    a.normalize(); pointIs(a,0.6f,0.8f);
    Vector2 tiny{0.000001f,0}; tiny.normalize(); pointIs(tiny,0,0);
    Vector2 threshold{std::bit_cast<float>(uint32_t{0x3727c5ac}),0}; threshold.normalize(); pointIs(threshold,1,0);
    Vector2 zero; zero.normalize(); pointIs(zero,0,0);
    zero.reverse(); assert(std::signbit(zero.x) && std::signbit(zero.y));
    Vector2 angle; angle.setAngleLen(0,3); pointIs(angle,3,0); assert(angle.angle()==0);
    Vector2 arithmetic{6,8}; arithmetic.div(2); arithmetic.mul(3); arithmetic.sub({1,2}); arithmetic.add({2,1}); pointIs(arithmetic,10,11);
    // Right-to-left segment has upward normal. Above the surface is positive.
    ColVector wall({10,20},{0,20}); pointIs(wall.normal,0,1); assert(wall.length==10);
    Vector2 p{5,20.5f}; auto hit=wall.checkCollideColRadius(p,1); assert(hit); pointIs(*hit,5,20); pointIs(p,5,20.5f);
    p={5,21}; assert(!wall.checkCollideColRadius(p,1)); // Radius equality excluded.
    p={11,20.5f}; assert(wall.checkCollideColRadius(p,1)); // Endpoint extension equality included.
    p={11.01f,20.5f}; assert(!wall.checkCollideColRadius(p,1));
    p={5,21}; assert(wall.checkCollideVel(p,{0,-2})); pointIs(p,5,20.5f);
    p={5,20}; assert(wall.checkCollideVel(p,{0,0})); pointIs(p,5,20.5f);
    p={5,21}; assert(!wall.checkCollideVel(p,{0,1})); pointIs(p,5,21);
    p={-3,21}; assert(wall.checkCollideVel(p,{0,-2})); pointIs(p,-3,20.5f);
    p={-3.01f,21}; assert(!wall.checkCollideVel(p,{0,-2})); pointIs(p,-3.01f,21);
    Vector2 relative{-5,0.5f}; hit=wall.checkCollideDirColRadius(relative,{0,-1},1); assert(hit); pointIs(*hit,5,20); pointIs(relative,-5,1);
    relative={-5,1}; assert(!wall.checkCollideDirColRadius(relative,{0,-1},1));
    relative={0,0.5f}; assert(wall.checkCollideDirColRadius(relative,{0,-1},1));
    relative={0.01f,0.5f}; assert(!wall.checkCollideDirColRadius(relative,{0,-1},1));
    hit=wall.checkCollideVelColRadius({5,22},{0,-4},1); assert(hit); pointIs(*hit,5,20);
    hit=wall.checkCollideVelColRadius({5,20.75f},{0,-0.25f},1); assert(hit); pointIs(*hit,5,20);
    ColVector diagonal({0,0},{3,4}); pointIs(diagonal.dir,0.6f,0.8f); pointIs(diagonal.normal,0.8f,-0.6f);
    p={1.9f,1.7f}; hit=diagonal.checkCollideColRadius(p,1); assert(hit); pointIs(*hit,1.5f,2);
    p={5,19}; assert(!wall.checkCollideVel(p,{0,-1})); pointIs(p,5,19); // Starts behind the one-sided face.
    p={5,21}; assert(wall.checkCollideVel(p,{0,-1})); pointIs(p,5,20.5f); // End exactly on face.
    relative={-5,0.5f}; assert(!wall.checkCollideDirColRadius(relative,{0,1},1)); pointIs(relative,-5,0.5f);
    const Vector2 sweepStart{5,22}, sweepVelocity{0,-4};
    (void)wall.checkCollideVelColRadius(sweepStart,sweepVelocity,1);
    pointIs(sweepStart,5,22); pointIs(sweepVelocity,0,-4);
    // Preserve original undefined-geometric IEEE outcomes; no invented zero guard.
    ColVector degenerate({1,1},{1,1}); assert(std::isnan(degenerate.dir.x) && std::isnan(degenerate.dir.y));
    relative={-5,0.5f}; hit=wall.checkCollideDirColRadius(relative,{1,0},1); assert(hit && !std::isfinite(hit->x));
    hit=wall.checkCollideVelColRadius({5,20.5f},{0,0},1); assert(hit && std::isnan(hit->x));
    bool rejected=false; try { wall.checkCollideVelColRadius({5,21},{0,-2},0); } catch(const std::invalid_argument&) { rejected=true; } assert(rejected);
}
