#pragma once
#include <cmath>
namespace heavymach {
// GobWorld init 0xb0a4 and accelerometer:didAccelerate: 0xb55c.
// Original 50 Hz sample filter uses promoted 0.1f and (1.0 - 0.1f).
struct TiltState {
    float x=0,y=0,z=0;
    static constexpr double interval=double(.02f);
    void sample(double ax,double ay,double az) {
        if (!std::isfinite(ax)||!std::isfinite(ay)||!std::isfinite(az)) return;
        constexpr double weight=double(.1f);
        x=float(ax*weight+double(x)*(1.0-weight));
        y=float(ay*weight+double(y)*(1.0-weight));
        z=float(az*weight+double(z)*(1.0-weight));
    }
};
}
