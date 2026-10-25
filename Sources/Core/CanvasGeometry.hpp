#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace heavymach {
// Game units remain unchanged. Only the visible horizontal extent changes.
struct CanvasGeometry {
 float width=480;
 static constexpr float height=320;
 float centerX()const{return width*.5f;}
 float hudOffsetX()const{return (width-480)*.5f;}
 int rasterWidth()const{return int(std::ceil(width));}
 static CanvasGeometry forDrawable(double width,double height,bool expanded){
  if(!expanded||width<=0||height<=0||!std::isfinite(width)||!std::isfinite(height))return {};
  return {float(std::max(480.0,320.0*width/height))};
 }
 static float checkedWidth(float width){
  if(!std::isfinite(width)||width<480||width>16384)throw std::invalid_argument("Canvas width outside supported range");
  return width;
 }
};
}
