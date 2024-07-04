/**
 * \file      AppDelegate.h
 * \brief     Declaration of the callbacks in ObjectivC for iOS
 * \details   The most eventhadlers will call the C interface in SLInterface.h
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

#import <UIKit/UIKit.h>

@class ViewController;

@interface AppDelegate : UIResponder<UIApplicationDelegate>

@property (strong, nonatomic) UIWindow*       window;
@property (strong, nonatomic) ViewController* viewController;

@end
