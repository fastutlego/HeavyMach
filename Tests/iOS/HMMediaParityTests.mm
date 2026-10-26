#import <XCTest/XCTest.h>
#import <CommonCrypto/CommonDigest.h>
#import "../../Sources/iOS/HMMedia.h"
@interface HMMediaParityTests : XCTestCase
@end
@implementation HMMediaParityTests
- (void)testAllOriginalImagesMatchCompatibilityDecoderPixels {
    NSURL *url=[NSBundle.mainBundle URLForResource:@"ImageDecodeParity" withExtension:@"json" subdirectory:@"Validation"];
    XCTAssertNotNil(url);if(!url)return;
    NSArray *records=[NSJSONSerialization JSONObjectWithData:[NSData dataWithContentsOfURL:url] options:0 error:nil];
    XCTAssertEqual(records.count,NSUInteger(695));
    HMMedia *media=[HMMedia new];
    for(NSDictionary *record in records)@autoreleasepool {
        NSString *name=record[@"name"];HMImage *image=[media image:name];XCTAssertNotNil(image,@"%@",name);if(!image)continue;
        XCTAssertEqual(CGImageGetWidth(image.CGImage),[record[@"width"] unsignedIntegerValue],@"%@",name);
        XCTAssertEqual(CGImageGetHeight(image.CGImage),[record[@"height"] unsignedIntegerValue],@"%@",name);
        NSData *rgba=HMOriginalImageRGBA(image);XCTAssertNotNil(rgba,@"%@",name);
        XCTAssertEqual(rgba.length,CGImageGetWidth(image.CGImage)*CGImageGetHeight(image.CGImage)*4,@"%@",name);
        unsigned char digest[CC_SHA256_DIGEST_LENGTH];CC_SHA256(rgba.bytes,(CC_LONG)rgba.length,digest);
        NSMutableString *hash=[NSMutableString new];for(unsigned char c:digest)[hash appendFormat:@"%02x",c];
        XCTAssertEqualObjects(hash,record[@"sha256"],@"%@",name);
        XCTAssertEqual([media image:name],image); // repeated loads retain the exact decoded bytes
    }
}
@end
