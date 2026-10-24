#include "../Sources/Core/TitleState.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
using namespace heavymach;
void runTitleStateTests(){
    TitleState title;size_t consumed=0;std::vector<uint32_t> values;
    auto random=[&](){assert(consumed<values.size());return values[consumed++];};
    // None of the title/episode intro frames, nor the exact six-second boundary,
    // may advance the game's shared RNG stream.
    for(int i=0;i<=360;++i)title.tick(double(i)/60,random);
    assert(consumed==0&&title.showTitle&&title.showEpisode);
    values={6};title.showStart=true;
    auto events=title.tick(6.01,random);
    assert(consumed==1&&title.showStart);
    assert(std::count(events.begin(),events.end(),TitleEvent::tapSound)==1);
    values.push_back(5);values.push_back(1);
    events=title.tick(6.02,random);assert(consumed==3&&!title.showStart&&events.empty());
    values.push_back(0);values.push_back(2);
    title.tick(6.03,random);assert(consumed==5&&title.showStart);
    title.tick(6+double(.2f),random);assert(consumed==5&&title.showStart);
    title.tick(6+double(1.2f),random);assert(consumed==5&&!title.showStart&&!title.startSound);
    title.tick(7.9,random);assert(consumed==5);
    values.push_back(9);
    events=title.tick(8,random);assert(consumed==6);
    assert(std::count(events.begin(),events.end(),TitleEvent::tapSound)==1);
    // Time-skipping by the first tap does not consume RNG; the next eligible
    // tick uses the same gate+optional-bit sequence, without prefetching.
    TitleState skipped;assert(!skipped.released(1));assert(skipped.released(1));
    size_t skipCalls=0;
    skipped.tick(1,[&](){++skipCalls;return 0u;});assert(skipCalls==0);
    skipped.tick(1.01,[&](){++skipCalls;return 0u;});assert(skipCalls==2&&skipped.showStart);
}
#ifdef HEAVYMACH_TITLE_TEST_MAIN
int main(){runTitleStateTests();std::cout<<"TitleState conditional RNG tests passed\n";}
#endif
