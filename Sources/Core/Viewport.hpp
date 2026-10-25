#pragma once
#include "CanvasGeometry.hpp"
#include <algorithm>
#include <cmath>
#include <optional>
#include <utility>
namespace heavymach {
// Original logical screen 480x320; the compatibility host fits the full drawable.
struct Viewport {
    double x, y, scale;
    double scaleY=0;
    double logicalWidth=480;
    static Viewport drawable(int width,int height,double nativeScale,double logicalWidth=480) {
        if(width<=0||height<=0||nativeScale<=0)return {0,0,0};
        const float aspect=float(logicalWidth)/320.f;
        int w=width,h=height;
        if(aspect<float(width)/float(height))w=int(std::round(float(height)*aspect));
        else h=int(std::round(float(width)/aspect));
        int left=(width-w)/2,bottom=(height-h)/2;
        return {left/nativeScale,(height-bottom-h)/nativeScale,w/(logicalWidth*nativeScale),h/(320.0*nativeScale),logicalWidth};
    }
    double verticalScale() const {return scaleY>0?scaleY:scale;}
    static Viewport fit(double x, double y, double width, double height,double logicalWidth=480) {
        double s=std::min(width/logicalWidth,height/320.0);
        return {x+(width-logicalWidth*s)/2,y+(height-320*s)/2,s,0,logicalWidth};
    }
    std::optional<std::pair<float,float>> logical(double px,double py) const {
        if (scale<=0) return std::nullopt;
        double gx=(px-x)/scale, gy=320-(py-y)/verticalScale();
        if (gx<0||gx>logicalWidth||gy<0||gy>320) return std::nullopt;
        return std::pair(float(gx),float(gy));
    }
};
}
