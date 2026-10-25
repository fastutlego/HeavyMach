#pragma once
#include <cstdint>
#include <functional>
#include <vector>
namespace heavymach {
enum class TitleEvent { titleIn, bangTitle, bangEpisode, tapSound };
// GuiTitle init/processTitle/onTap from original REA exports. Tick at 60 Hz.
struct TitleState {
    float bgScale=3.2f,bgRotate=-0.2f,camX=400,camY=-200;
    float titleScale=50,episodeScale=40,startTime=0;
    bool showTitle=false,showEpisode=false,showStart=false;
    bool titleSound=false,episodeSound=false,startSound=false;
    using Random31=std::function<uint32_t()>;
    // Draw lazily: zero calls outside blink phase, one gate draw, optionally one bit draw.
    std::vector<TitleEvent> tick(double time,const Random31& random31);
    // Deterministic compatibility adapter for existing value-based core fixtures.
    std::vector<TitleEvent> tick(double time,uint32_t randomA,uint32_t randomB);
    bool released(double time); // true => changeScreen:3; first tap skips timing.
};
}
