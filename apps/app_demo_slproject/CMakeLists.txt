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
    ANDROID_APP_DIR "android/app"

    # On iOS, set the path to the Info.plist template as well as some other properties.
    IOS_INFO_PLIST "ios/plist.in"
    IOS_DISPLAY_NAME "SL Demo"
    IOS_COPYRIGHT "Copyright Berner Fachhochschule (Marcus Hudritsch)"
    IOS_ICON_NAME "AppIcon"

    # Define additional Xcode app resources for iOS.
    IOS_RESOURCES
    "ios/Base.lproj/ViewController_iPad.xib"
    "ios/Base.lproj/ViewController_iPhone.xib"
    "ios/LaunchScreenSLDemo.storyboard"
    "ios/Images/Images.xcassets"
    "ios/Images/LaunchImage_1024x768.png"
)
