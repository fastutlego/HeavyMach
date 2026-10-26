#pragma once
#include "../Core/AutoAim.hpp"
#include <algorithm>
#include <array>
#include <cstring>

namespace heavymach {
// Presentation only: no targeting, input, simulation, allocation, or file writes.
struct AimHUDLabel {
    const char* text;
    float x,y,anchorX,anchorY,elbowX,elbowY,endX;
    bool flash;
};
inline AimHUDLabel aimHUDLabel(float left,float right,float top,float width,float height,AimStatus status,float elapsed) {
    const char* text=status==AimStatus::ready?"LOCKED":status==AimStatus::blocked?"BLOCKED":"TRACK";
    float length=float(std::strlen(text))*6-2;
    bool onRight=right+length+24<width;
    float anchorX=std::clamp(onRight?right:left,4.f,width-4);
    float anchorY=std::clamp(top,4.f,height-20);
    float x=std::clamp(onRight?anchorX+12:anchorX-12-length,6.f,width-length-6);
    float y=std::clamp(anchorY+15,76.f,height-12);
    float elbowX=onRight?x:x+length;
    float elbowY=y-3;
    float progress=std::clamp(elapsed/.15f,0.f,1.f);
    float endX=onRight?x+length:x;
    return {text,x,y,anchorX,anchorY,
        anchorX+(elbowX-anchorX)*progress,anchorY+(elbowY-anchorY)*progress,
        anchorX+(endX-anchorX)*progress,elapsed>=.15f&&elapsed<.24f};
}
struct AimHUDStrokes { std::array<float,512> points{}; size_t size=0; };
inline AimHUDStrokes aimHUDText(const AimHUDLabel& label) {
    AimHUDStrokes out;
    float x=label.x;
    // Nine-point stencil grid (top row 012, middle 345, bottom 678).
    for(const char* c=label.text;*c;++c,x+=6){
        const char* strokes="";
        switch(*c){
            case 'L':strokes="0668";break;
            case 'O':strokes="02288660";break;
            case 'C':strokes="200668";break;
            case 'K':strokes="063238";break;
            case 'E':strokes="20063568";break;
            case 'D':strokes="0115577660";break;
            case 'B':strokes="0668855335522003";break;
            case 'T':strokes="0217";break;
            case 'R':strokes="066002255338";break;
            case 'A':strokes="60022835";break;
        }
        for(const char* s=strokes;*s;++s){int point=*s-'0';out.points[out.size++]=x+(point%3)*2;out.points[out.size++]=label.y+6-(point/3)*3;}
    }
    return out;
}
}
