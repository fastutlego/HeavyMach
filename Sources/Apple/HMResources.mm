#import "HMResources.h"
NSBundle *HMGameResourceBundle(void) {
    static NSBundle *bundle;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        NSURL *url=[NSBundle.mainBundle URLForResource:@"HeavyMachAssets" withExtension:@"bundle"];
        bundle=url?[NSBundle bundleWithURL:url]:nil;
        NSCAssert(bundle, @"HeavyMachAssets.bundle is missing from the app");
    });
    return bundle;
}
