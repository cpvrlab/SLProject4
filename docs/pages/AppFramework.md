\page app-framework The %App Framework

\section overview Overview

SLProject provides a framework for quickly setting up platform-independent apps that run on Windows, macOS, Linux, Android, iOS, and the Web. The app framework is divided into three layers:

- The **App Layer** contains the code of your app. At least one code file has to contain the apps main function and some configuration code, and callbacks for hooking into the SLProject systems. In our example apps these files end with *Main.cpp. The app code also must compile the AppCommon class that is the owner of the central instances of the SLProject. The central interface that every app uses is the `App.h` header, which is implemented by the platform layer.
- The **Platform Layer** contains all platform-specific code for creating the rendering surface, setting up event handlers, etc. Every platform has its own implementation of the `App.h`. The implementation corresponding to the target platform is selected at compile time by CMake. The platform layer uses `SLInterface.h` to access SLProject modules. The platform-specific code is C/C++ for the GLFW platforms (Windows, MacOS & Linux), C/C++ and Java for Android, C/C++ and JavaScript for Emscripten (running in the web browser) and ObjectiveC for iOS. All platform-specific code is under `apps/source/platforms`.
- The **SL Layer** contains all the SLProject libraries. This is where all inputs get processed, the updates get done and finally the 3D, 2D and UI rendering is done. The shown classes in the diagram is only a small section of the framework. Please see the [introduction page](https://cpvrlab.github.io/SLProject4/introduction.html) for a deeper overview.

<div style="width: 100%; justify-content: center; display: flex; margin: 20pt 0">
    <img src="images/app_framework.svg" width="70%">
</div>

\section usage Usage

\subsection cpp C++

The main C++ file of an app looks something like this:

```cpp
#include <App.h>
// Other includes...

static SLScene* createScene(SLSceneID sceneID)
{
    return new AppExampleScene();
}

static void buildGui(SLScene* s, SLSceneView* sv)
{
    // ...
}

int SL_MAIN_FUNCTION(int argc, char* argv[])
{
    App::Config config;
    config.argc         = argc,
    config.argv         = argv;
    config.windowWidth  = 640;
    config.windowHeight = 480;
    config.windowTitle  = "SLProject Example App";
    config.onNewScene   = createScene;
    config.onGuiBuild   = buildGui;
    // More config...

    return App::run(config);
}
```

The main function is defined using a macro that expands to the name of the main function on the current platform. This is usually `main`, but on Android it expands to `slAndroidMain`, a fake main function that is called from the JNI to initialize the application because there is no actual `main` on Android.

Inside the main function, an `App::Config` instance is created. This configuration specifies things like the window size, the ID of the start scene, or a callback for building the GUI. `App::run` is then called with this config to start the app. Every platform implements its own version of this function. The various implementations can be found in `apps/source/platforms`.
Here's a list of all of them:

- `AppGLFW.cpp` (Windows, macOS, Linux)
- `AppEmscripten.cpp` (Web)
- `AppAndroid.cpp` (Android)
- `AppIOS.mm` (iOS)

An implementation of `App::run` typically sets up the rendering surface, registers event handlers, loads core assets, starts the event loop, and returns when the program terminates. Only the Android version returns immediately because the event loop is handled by Java code and interacts with the app via the JNI.

For more information on creating a new app, see [this wiki page](https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App).

\subsection cmake CMake

New apps are added by calling the custom CMake function `sl_add_app`. This function is defined in `apps/CMakeLists.txt` and takes care of creating a new target and setting source files, headers, compile options, linked libraries, properties, etc. depending on the target platform. The function has parameters for specifying things like the name of the target, the platforms the app supports, additional source files (e.g. for scenes), the directory of the Android project, or iOS resources.

`sl_add_app` uses the CMake variable `SL_PLATFORM` to decide which platform is currently being targeted. This variable is set in the top-level `CMakeLists.txt` based on the current target system. The possible platforms are:

- `GLFW` (Windows, macOS, Linux)
- `EMSCRIPTEN` (Web)
- `ANDROID` (Android)
- `IOS` (iOS)

\section scene-loading Scene Loading

A scene is loaded by setting the value of `AppCommon::sceneToLoad` to the ID of the new scene. Inside the event loop of every platform, we check whether this variable has a value and switch to the new scene if so:

```cpp
if (AppCommon::sceneToLoad)
{
    slSwitchScene(sv, *AppCommon::sceneToLoad);
    AppCommon::sceneToLoad = {};
}
```

Scene switching is implemented inside `AppLoad::switchScene`. This function calls the callback `AppConfig::onNewScene` to create a new scene instance. The callback is specified by the app and returns an instance of a class derived from `SLScene` (e.g. `AppDemoSceneMinimal`). After creating the new scene instance, we switch to it in two stages:

1. Register assets to be loaded. We call the overloaded `SLScene::registerAssetsToLoad` so it can register tasks on the `SLAssetLoader` to be executed asynchronously in a different thread. Meanwhile, the event loop on the main thread continues running so we can for example render a loading screen. Every frame, `SLAssetLoader::checkIfAsyncLoadingIsDone` is called to check whether the worker thread is done. When that happens, the next stage is triggered. Note: All I/O operations have to be registered here because they cannot be executed synchronously on the main thread on the [Emscripten platform](https://cpvrlab.github.io/SLProject4/emscripten.html).
2. Assemble the scene. When the asset loader is done, we call the overloaded `SLScene::assemble` function that uses the loaded assets to assemble the scene. After assembly, `AppCommon::scene` is set to the new scene and the scene switch is completed.
