#pragma once
#include <cmath>
#include <numbers>

namespace heavymach {
// QobButton 0x82b8, 0x850c, 0x87f8. Times are the original global clock.
struct HMButtonState {
    enum class Event { none, push, pop, release };
    float pushDepth=2, pushDelay=.1f, releaseHeight=42, releaseDelay=.4f;
    double clickTime=0, releaseTime=0;
    bool pressed=false;
    void releaseAction(double time) {if(!pressed&&releaseTime==0)releaseTime=time;}
    Event touch(bool inside,int state,double time) {
        // UIKit cancellation has no original counterpart: release without PopButton.
        if(state==3){pressed=false;if(clickTime!=0){clickTime=0;releaseTime=time;return Event::release;}return Event::none;}
        if(state==2){
            pressed=false;
            if(!inside)return Event::none;
            if(clickTime!=0){clickTime=0;releaseTime=time;}
            return Event::pop;
        }
        if(!inside){
            pressed=false;
            if(clickTime!=0){clickTime=0;releaseTime=time;return Event::release;}
            return Event::none;
        }
        if(clickTime==0){clickTime=time;releaseTime=0;}
        pressed=true;
        return state==0?Event::push:Event::none;
    }
    float displacement(double time) {
        if(clickTime!=0){
            double elapsed=time-clickTime;
            if(elapsed>=double(pushDelay))return pushDepth;
            return std::sin(float((elapsed/double(pushDelay))*(std::numbers::pi/2)))*pushDepth;
        }
        if(releaseTime!=0){
            double elapsed=time-releaseTime;
            if(elapsed<double(releaseDelay)){
                double angle=(elapsed/double(releaseDelay))*std::numbers::pi;
                float wave=std::sin(float(angle+angle));
                return float(double(wave)*(double(releaseHeight)*(double(releaseDelay)-elapsed)));
            }
            releaseTime=0;
        }
        return 0;
    }
    float yOffset(double time,bool active=true) {
        float offset=displacement(time);
        return !active||pressed?-offset:offset;
    }
};
}
