/**
 * \file      AppDelegate.mm
 * \brief     Implementation of the callbacks in ObjectivC for iOS
 * \details   The most eventhadlers call the C interface in SLInterface.h
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 *            For more info on how to build for the iOS platform see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Build-on-MacOS-with-XCode-for-iOS
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#import "AppDelegate.h"
#import "ViewController.h"

#include <SLInterface.h>

@implementation AppDelegate

@synthesize window         = _window;
@synthesize viewController = _viewController;

- (void)dealloc
{
    //[_window release];
    //[_viewController release];
    // [super dealloc];
}

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPhone" bundle:nil];
    }
    else
    {
        self.viewController = [[ViewController alloc] initWithNibName:@"ViewController_iPad" bundle:nil];
    }
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication*)application
{
    /*
     Sent when the application is about to move from active to inactive state.
     This can occur for certain types of temporary interruptions (such as an
     incoming phone call or SMS message) or when the user quits the application
     and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down
     OpenGL ES frame rates. Games should use this method to pause the game.
     */
    printf("applicationWillResignActive\n");
}

- (void)applicationDidEnterBackground:(UIApplication*)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers,
     and store enough application state information to restore your application to
     its current state in case it is terminated later.
     If your application supports background execution, this method is called instead
     of applicationWillTerminate: when the user quits.
     */

    printf("applicationDidEnterBackground\n");
    slTerminate();
    exit(0);
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
    /*
     Called as part of the transition from the background to the inactive state;
     here you can undo many of the changes made on entering the background.
     */

    printf("applicationWillEnterForeground\n");
}

- (void)applicationDidBecomeActive:(UIApplication*)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application
     was inactive. If the application was previously in the background,
     optionally refresh the user interface.
     */

    printf("applicationDidBecomeActive\n");
}

- (void)applicationWillTerminate:(UIApplication*)application
{
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */

    printf("applicationWillTerminate\n");
}

@end
