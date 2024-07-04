/**
 * \file      AppIOS.cpp
 * \brief     App::run implementation from App.h for the iOS platform
 * \details   For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 *            For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info on how to build for the iOS platform see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Build-on-MacOS-with-XCode-for-iOS
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <App.h>

#import "AppDelegate.h"
#import <UIKit/UIKit.h>

//-----------------------------------------------------------------------------
// Global variables
App::Config App::config; //!< The configuration set in App::run

//-----------------------------------------------------------------------------
int App::run(Config config)
{
    App::config = config;

    @autoreleasepool
    {
        return UIApplicationMain(config.argc,
                                 config.argv,
                                 nil,
                                 NSStringFromClass([AppDelegate class]));
    }
}
//-----------------------------------------------------------------------------
