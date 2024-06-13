//
//  AppIOS.m
//  Created by Marcus Hudritsch on 30.11.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

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
