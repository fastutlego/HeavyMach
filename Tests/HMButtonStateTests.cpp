#include "../Sources/iOS/HMButtonState.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using heavymach::HMButtonState;
namespace {void near(float a,float b){assert(std::fabs(a-b)<.00001f);}}
int main(){
    HMButtonState b;
    assert(b.touch(true,0,10)==HMButtonState::Event::push);
    near(b.yOffset(10),0);
    near(b.yOffset(10+double(b.pushDelay)/2),-std::sqrt(2.f));
    near(b.yOffset(10+double(b.pushDelay)),-2);
    // Callback is synchronous, with no animation-delay gate.
    assert(b.touch(true,2,10.02)==HMButtonState::Event::pop);
    assert(!b.pressed&&b.clickTime==0&&b.releaseTime==10.02);
    near(b.yOffset(10.02+double(b.releaseDelay)/4),42.f*b.releaseDelay*.75f);
    near(b.yOffset(10.02+double(b.releaseDelay)*.75),-42.f*b.releaseDelay*.25f);
    near(b.yOffset(10.02+double(b.releaseDelay)),0);assert(b.releaseTime==0);
    assert(b.touch(true,0,11)==HMButtonState::Event::push);
    assert(b.touch(false,1,11.01)==HMButtonState::Event::release);
    assert(!b.pressed&&b.clickTime==0);
    assert(b.touch(true,1,11.02)==HMButtonState::Event::none);assert(b.pressed);
    assert(b.touch(true,2,11.03)==HMButtonState::Event::pop);
    // Original PopButton also fires on an inside end without a prior inside begin.
    HMButtonState endOnly;assert(endOnly.touch(true,2,12)==HMButtonState::Event::pop);
    assert(endOnly.releaseTime==0);
    b.touch(true,0,13);assert(b.touch(false,3,13.01)==HMButtonState::Event::release);
    assert(!b.pressed&&b.clickTime==0);
    b.releaseTime=0;b.releaseAction(14);assert(b.releaseTime==14);
    b.releaseAction(15);assert(b.releaseTime==14);
    // All menu/result controls explicitly use zero displacement/duration.
    HMButtonState still;still.pushDepth=still.releaseHeight=still.releaseDelay=0;
    still.touch(true,0,20);near(still.yOffset(20.05),0);
    still.touch(true,2,20.05);near(still.yOffset(20.05),0);assert(still.releaseTime==0);
    // Upgrade OK uses depth1/height1/duration.1, unlike the generic defaults.
    HMButtonState upgrade;upgrade.pushDepth=upgrade.releaseHeight=1;upgrade.releaseDelay=.1f;
    upgrade.touch(true,0,30);near(upgrade.yOffset(30.2),-1);
    upgrade.touch(true,2,30.2);near(upgrade.yOffset(30.2+double(upgrade.releaseDelay)/4),.075f);
    std::cout<<"HMButtonState timing and touch transitions passed\n";
}
