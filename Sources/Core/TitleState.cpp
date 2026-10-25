#include "TitleState.hpp"
#include <cmath>
namespace heavymach {
std::vector<TitleEvent> TitleState::tick(double time,const Random31& random31) {
    std::vector<TitleEvent> events;
    if (bgRotate<0) bgRotate+=(0.01f-bgRotate)/60; else bgRotate=0;
    if (bgScale>1) bgScale+=(0.99f-bgScale)/50; else bgScale=1;
    camX+=(0-camX)/50; camY+=(0-camY)/50;
    if (!titleSound && double(startTime+2.2f)<time) {events.push_back(TitleEvent::titleIn);titleSound=true;}
    if (!episodeSound && double(startTime+3.7f)<time) {events.push_back(TitleEvent::titleIn);episodeSound=true;}
    if (titleScale!=1 && double((startTime+2.2f)+0.3f)<time) {
        if (titleScale>1) titleScale-=5;
        else if (titleScale<1) {events.push_back(TitleEvent::bangTitle);titleScale=1;}
        showTitle=true;
    }
    if (episodeScale!=1 && double((startTime+3.7f)+0.3f)<time) {
        if (episodeScale>1) episodeScale-=5;
        else {events.push_back(TitleEvent::bangEpisode);episodeScale=1;}
        showEpisode=true;
    }
    double begin=double(startTime+6.0f);
    if (begin<time) {
        float phase=float(std::fmod(double(float(time-begin)),2.0));
        if (phase<0.2f) {
            if (!startSound) {events.push_back(TitleEvent::tapSound);startSound=true;}
            if (random31()%10<=5) showStart=(~random31()&1)!=0;
        } else if (phase<1.2f) showStart=true;
        else if (startSound) {startSound=false;showStart=false;}
    }
    return events;
}
std::vector<TitleEvent> TitleState::tick(double time,uint32_t a,uint32_t b) {
    unsigned calls=0;
    return tick(time,[&]{return calls++==0?a:b;});
}
bool TitleState::released(double time) {
    if (double(startTime+6.0f)>time) {startTime=float(time-6.0);return false;}
    return true;
}
}
