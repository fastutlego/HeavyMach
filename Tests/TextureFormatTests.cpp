#include "../Sources/Core/TextureFormat.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace heavymach;
int main() {
    for(unsigned alpha=0;alpha<=6;++alpha) {
        assert(!originalTextureUsesRGB565(false,alpha));
        assert(originalTextureUsesRGB565(true,alpha)==(alpha==0||alpha==5||alpha==6));
    }
    assert(packOriginalRGB565(255,0,0)==0xf800);
    assert(packOriginalRGB565(0,255,0)==0x07e0);
    assert(packOriginalRGB565(0,0,255)==0x001f);
    assert(packOriginalRGB565(7,3,7)==0);
    assert(packOriginalRGB565(8,4,8)==0x0821);
    auto changed=originalRGB565ToRGB8(packOriginalRGB565(123,45,67));
    assert(changed.r==123&&changed.g==45&&changed.b==66); // previous 8888 bridge kept B=67
    for(unsigned packed=0;packed<65536;++packed) {
        auto c=originalRGB565ToRGB8(uint16_t(packed));
        assert(packOriginalRGB565(c.r,c.g,c.b)==packed);
        assert(std::abs(c.r-((packed>>11)&31)*255.0/31)<=.5);
        assert(std::abs(c.g-((packed>>5)&63)*255.0/63)<=.5);
        assert(std::abs(c.b-(packed&31)*255.0/31)<=.5);
    }
    std::cout<<"RGB565: alpha branch, ARM packing, 65536 normalized colors passed\n";
}
