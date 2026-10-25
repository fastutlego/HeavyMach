#import <XCTest/XCTest.h>
#include "../../Sources/iOS/HMCompatibilityPixels.hpp"
#include <algorithm>
@interface HMCompatibilityPixelsTests : XCTestCase
@end
@implementation HMCompatibilityPixelsTests
- (void)testBitmapUploadMatchesCompatibilityNormalBlendForAllByteAndAlphaValues {
    // Independent slow scalar oracle: the compatibility CGContext's decode,
    // blend_premultiplied/NoneSkipLast and encode operations on a cleared bitmap.
    for(int alpha=0;alpha<256;++alpha)for(int value=0;value<256;++value) {
        uint8_t pixels[]={uint8_t(value),uint8_t(255-value),uint8_t(alpha),uint8_t(alpha)};
        uint8_t input[4];std::copy_n(pixels,4,input);
        heavymach::compatibilityBitmapPixels(pixels,1);
        float fgAlpha=float(alpha)/255.f;
        for(int c=0;c<3;++c) {
            float fg=::powf(float(input[c])/255.f,2.2f);
            float rgbaBlend=fg*(1.f-0.f)+(0.f*fg)+(0.f*(1.f-fgAlpha));
            uint8_t expectedRGBA=uint8_t(::powf(rgbaBlend,1.f/2.2f)*255.f);
            uint8_t expectedNoAlpha=uint8_t(::powf(fg,1.f/2.2f)*255.f);
            XCTAssertEqual(pixels[c],expectedRGBA);
            XCTAssertEqual(pixels[c],expectedNoAlpha);
        }
        XCTAssertEqual(pixels[3],uint8_t((fgAlpha+0.f*(1.f-fgAlpha))*255.f));
    }
}
@end
