#include "../Sources/Core/AudioContract.hpp"
#include <cassert>
#include <limits>
using namespace heavymach;
int main(){
 assert(originalSoundMaxDistance("SFX_ExplGround")==800);
 assert(originalSoundMaxDistance("Wpn_Laser")==800);
 assert(originalSoundMaxDistance("Loading_Close")==std::numeric_limits<float>::max());
 assert(originalSoundMaxDistance("Title_Bang")==std::numeric_limits<float>::max());
 for(float x:{-1.f,-.5f,0.f,.5f,1.f})assert(originalSoundGain("Wpn_Gun01",x)==1);
 assert(originalSoundGain("Wpn_Gun01",2)==.5f);
 assert(originalSoundGain("Wpn_Gun01",-4)==.25f);
 assert(originalSoundGain("Wpn_Gun01",1000)==1.f/800.f);
 assert(originalSoundGain("Title_Bang",1000)==.001f);
 assert(originalSoundGain("Wpn_Gun01",std::numeric_limits<float>::quiet_NaN())==0);
}
