sl_add_app(
    TARGET "app-Demo-SLProject"
    
    PLATFORMS
    "GLFW"
    "EMSCRIPTEN"
    "ANDROID"
    "IOS"
    
    COMPILE_DEFINITIONS
    "SL_STARTSCENE=SID_ShaderIBL"
    
    HEADERS
    "source/*.h"
    "source/scenes/*.h"
    
    SOURCES
    "source/*.cpp"
    "source/scenes/*.cpp"
    
    INCLUDE_DIRECTORIES
    "source/"
    "source/scenes/"

    # On Android, set the path to the `app` directory
    ANDROID_APP_DIR "${SL_PROJECT_ROOT}/apps/source/platforms/android/example_project/app"

    # On iOS, set the path to the Info.plist template as well as some other properties.
    IOS_INFO_PLIST "${SL_PROJECT_ROOT}/apps/source/platforms/ios/example_project/plist.in"
    IOS_DISPLAY_NAME "SL Demo"
    IOS_COPYRIGHT "Copyright Berner Fachhochschule (Marcus Hudritsch)"
    IOS_ICON_NAME "AppIcon"

    # Define additional Xcode app resources for iOS.
    IOS_RESOURCES
    "${SL_PROJECT_ROOT}/apps/source/platforms/ios/example_project/Base.lproj/ViewController_iPad.xib"
    "${SL_PROJECT_ROOT}/apps/source/platforms/ios/example_project/Base.lproj/ViewController_iPhone.xib"
    "${SL_PROJECT_ROOT}/apps/source/platforms/ios/example_project/LaunchScreenSLDemo.storyboard"
    "${SL_PROJECT_ROOT}/apps/source/platforms/ios/example_project/Images/Images.xcassets"
    "${SL_PROJECT_ROOT}/apps/source/platforms/ios/example_project/Images/LaunchImage_1024x768.png"
)
