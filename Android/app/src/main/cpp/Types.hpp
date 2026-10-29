#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
namespace hm {
using CGFloat=double;using NSInteger=int;
struct CGPoint {double x=0,y=0;};struct CGSize {double width=0,height=0;};struct CGRect {CGPoint origin;CGSize size;};
inline constexpr CGPoint CGPointZero{};inline constexpr CGRect CGRectZero{};
inline CGPoint CGPointMake(double x,double y){return {x,y};}
inline CGRect CGRectMake(double x,double y,double w,double h){return {{x,y},{w,h}};}
inline double CGRectGetMinX(CGRect r){return r.origin.x;}inline double CGRectGetMinY(CGRect r){return r.origin.y;}
inline double CGRectGetMaxX(CGRect r){return r.origin.x+r.size.width;}inline double CGRectGetMaxY(CGRect r){return r.origin.y+r.size.height;}
inline double CGRectGetMidX(CGRect r){return r.origin.x+r.size.width/2;}inline double CGRectGetMidY(CGRect r){return r.origin.y+r.size.height/2;}
inline bool CGRectIsEmpty(CGRect r){return r.size.width<=0||r.size.height<=0;}
inline bool CGRectEqualToRect(CGRect a,CGRect b){return a.origin.x==b.origin.x&&a.origin.y==b.origin.y&&a.size.width==b.size.width&&a.size.height==b.size.height;}
inline CGRect CGRectOffset(CGRect r,double x,double y){r.origin.x+=x;r.origin.y+=y;return r;}
inline bool originalButtonContains(CGRect r,CGPoint p){return float(p.x)>float(r.origin.x)&&float(p.x)<=float(CGRectGetMaxX(r))&&float(p.y)>float(r.origin.y)&&float(p.y)<=float(CGRectGetMaxY(r));}
template<class...Args>std::string fmt(const char* f,Args...args){int n=std::snprintf(nullptr,0,f,args...);if(n<0)throw std::runtime_error("Text formatting failed");std::vector<char>b(n+1);std::snprintf(b.data(),b.size(),f,args...);return {b.data(),size_t(n)};}
}
