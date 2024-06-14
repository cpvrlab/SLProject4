sl_add_app(
    TARGET "app-Demo-Node"
    
    PLATFORMS
    "GLFW"
    "EMSCRIPTEN"
    "IOS"
    "ANDROID"
    
    HEADERS "*.h"
    SOURCES "*.cpp"
    INCLUDE_DIRECTORIES "."

    # On Android, set the path to the `app` directory
    ANDROID_APP_DIR "${SL_PROJECT_ROOT}/apps/app_demo_slproject/android/app"

    # On iOS, set the path to the Info.plist template as well as some other properties.
    IOS_INFO_PLIST "${SL_PROJECT_ROOT}/apps/app_demo_slproject/ios/plist.in" 
    IOS_DISPLAY_NAME "SL Nodes"
    IOS_COPYRIGHT "Copyright Berner Fachhochschule (Marcus Hudritsch)"
    IOS_ICON_NAME "AppIcon"

    # Define additional Xcode app resources for iOS.
    IOS_RESOURCES
    "${SL_PROJECT_ROOT}/apps/app_demo_slproject/ios/Base.lproj/ViewController_iPad.xib"
    "${SL_PROJECT_ROOT}/apps/app_demo_slproject/ios/Base.lproj/ViewController_iPhone.xib"
    "${SL_PROJECT_ROOT}/apps/app_demo_slproject/ios/LaunchScreenSLDemo.storyboard"
    "${SL_PROJECT_ROOT}/apps/app_demo_slproject/ios/Images/Images.xcassets"
    "${SL_PROJECT_ROOT}/apps/app_demo_slproject/ios/Images/LaunchImage_1024x768.png"
)
