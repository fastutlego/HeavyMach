#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace heavymach {
// Keep original world units; expand the visible extent for each aspect ratio.
struct CanvasGeometry {
 float width=480;
 float height=320;
 float centerX()const{return width*.5f;}
 float hudOffsetX()const{return (width-480)*.5f;}
 int rasterWidth()const{return int(std::ceil(width));}
 static CanvasGeometry forDrawable(double width,double height,bool expanded){
  if(!expanded||width<=0||height<=0||!std::isfinite(width)||!std::isfinite(height))return {};
  return {float(std::max(480.0,320.0*width/height)),float(std::max(320.0,480.0*height/width))};
 }
 static float checkedWidth(float width){
  if(!std::isfinite(width)||width<480||width>16384)throw std::invalid_argument("Canvas width outside supported range");
  return width;
 }
};
}
