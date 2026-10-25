#import <UIKit/UIKit.h>
#import "HMGameView.h"
@interface HMViewController : UIViewController
@end
@implementation HMViewController
- (void)loadView {self.view=[[HMGameView alloc] initWithFrame:UIScreen.mainScreen.bounds];}
- (BOOL)prefersStatusBarHidden {return YES;}
- (UIInterfaceOrientationMask)supportedInterfaceOrientations {return UIInterfaceOrientationMaskLandscape;}
@end
@interface HMSceneDelegate : UIResponder <UIWindowSceneDelegate>
@property(nonatomic,strong) UIWindow *window;
@end
@implementation HMSceneDelegate
- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)options {
    if (![scene isKindOfClass:UIWindowScene.class]) return;
    self.window=[[UIWindow alloc] initWithWindowScene:(UIWindowScene *)scene];
    self.window.rootViewController=[HMViewController new]; [self.window makeKeyAndVisible];
}
- (void)sceneDidBecomeActive:(UIScene *)scene {[(HMGameView *)self.window.rootViewController.view setApplicationActive:YES];}
- (void)sceneWillResignActive:(UIScene *)scene {[(HMGameView *)self.window.rootViewController.view setApplicationActive:NO];}
- (void)sceneDidEnterBackground:(UIScene *)scene {[(HMGameView *)self.window.rootViewController.view setApplicationActive:NO];}
@end
@interface HMAppDelegate : UIResponder <UIApplicationDelegate>
@end
@implementation HMAppDelegate
- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)session options:(UISceneConnectionOptions *)options {
    UISceneConfiguration *config=[[UISceneConfiguration alloc] initWithName:@"HeavyMach" sessionRole:session.role];
    config.delegateClass=HMSceneDelegate.class; return config;
}
@end
int main(int argc,char **argv) {@autoreleasepool {return UIApplicationMain(argc,argv,nil,NSStringFromClass(HMAppDelegate.class));}}
