/**
 * \file      AppEmscripten.cpp
 * \date      June 2024
 * \brief     App::run implementation from App.h for the Emscripten platform
 * \details   The functions implement mostly the callbacks for the platform
 *            that are forwarded to the C interface in SLInterface.
 *            For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 *            For more info about the Emscripten platform see:
 *            https://cpvrlab.github.io/SLProject4/emscripten.html
 *            For more info on how to set up the Emscripten platform see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Build-for-the-web-with-Emscripten
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <App.h>
#include <SLGLState.h>
#include <SLEnums.h>
#include <SLInterface.h>
#include <AppCommon.h>
#include <SLAssetManager.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <CVCapture.h>
#include <Profiler.h>
#include <SLAssetLoader.h>

#include <emscripten.h>
#include <emscripten/em_asm.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

//-----------------------------------------------------------------------------
// Global variables
App::Config    App::config;              //!< The configuration set in App::run
static SLint   svIndex;                  //!< Scene view index
static SLint   startX;                   //!< start position x in pixels
static SLint   startY;                   //!< start position y in pixels
static SLint   mouseX;                   //!< Last mouse position x in pixels
static SLint   mouseY;                   //!< Last mouse position y in pixels
static SLVec2i touch2;                   //!< Last finger touch 2 position in pixels
static SLVec2i touchDelta;               //!< Delta between two fingers in x
static SLint   lastWidth;                //!< Last window width in pixels
static SLint   lastHeight;               //!< Last window height in pixels
static int     canvasWidth;              //!< Width of the HTML canvas
static int     canvasHeight;             //!< Height of the HTML canvas
static int     lastTouchDownX;           //!< X coordinate of last touch down
static int     lastTouchDownY;           //!< Y coordinate of last touch down
static double  lastTouchDownTimeMS;      //!< Time of last touch down in milliseconds
static long    animationFrameID = 0;     //!< ID of the current JavaScript animation frame
static SLbool  coreAssetsLoaded = false; //!< Indicates whether core assets can be used

//-----------------------------------------------------------------------------
// Function forward declarations
static SLbool            onPaint();
static void              updateCanvas();
static void              onLoadingCoreAssets();
static EM_BOOL           onAnimationFrame(double time, void* userData);
static EMSCRIPTEN_RESULT onMousePressed(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL           onMouseReleased(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL           onMouseDoubleClicked(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL           onMouseMove(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL           onMouseWheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData);
static EM_BOOL           onKeyPressed(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
static EM_BOOL           onKeyReleased(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
static EM_BOOL           onTouchStart(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
static EM_BOOL           onTouchEnd(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
static EM_BOOL           onTouchMove(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
static const char*       onUnload(int eventType, const void* reserved, void* userData);
static SLint             convertToCanvasCoordinate(SLint coordinate);
static SLKey             mapKeyToSLKey(unsigned long key);
static SLKey             mapModifiersToSLModifiers(bool shiftDown, bool ctrlDown, bool altDown);
static SLKey             mapModifiersToSLModifiers(const EmscriptenMouseEvent* mouseEvent);
static SLKey             mapModifiersToSLModifiers(const EmscriptenKeyboardEvent* keyEvent);

//-----------------------------------------------------------------------------
//! App::run implementation from App.h for the Emscripten platform
int App::run(Config config)
{
    App::config = config;

    canvasWidth  = EM_ASM_INT(return window.devicePixelRatio * window.innerWidth);
    canvasHeight = EM_ASM_INT(return window.devicePixelRatio * window.innerHeight);
    updateCanvas();

    EmscriptenWebGLContextAttributes attributes;
    emscripten_webgl_init_context_attributes(&attributes);
    attributes.enableExtensionsByDefault = true;
    attributes.antialias                 = false;
    attributes.depth                     = true;
    attributes.stencil                   = true;
    attributes.alpha                     = true;
    attributes.majorVersion              = 2;
    attributes.minorVersion              = 0;
    attributes.preserveDrawingBuffer     = true;

    auto context = emscripten_webgl_create_context("#canvas", &attributes);
    if (context > 0)
        SL_LOG("WebGL context created.");
    else
        SL_EXIT_MSG("Failed to create WebGL context.");

    EMSCRIPTEN_RESULT result = emscripten_webgl_make_context_current(context);
    if (result == EMSCRIPTEN_RESULT_SUCCESS)
        SL_LOG("WebGL context made current.");
    else
        SL_EXIT_MSG("Failed to make WebGL context current.");

    emscripten_set_mousedown_callback("#canvas", nullptr, false, onMousePressed);
    emscripten_set_mouseup_callback("#canvas", nullptr, false, onMouseReleased);
    emscripten_set_dblclick_callback("#canvas", nullptr, false, onMouseDoubleClicked);
    emscripten_set_mousemove_callback("#canvas", nullptr, false, onMouseMove);
    emscripten_set_wheel_callback("#canvas", nullptr, false, onMouseWheel);
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, false, onKeyPressed);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, false, onKeyReleased);
    emscripten_set_touchstart_callback("#canvas", nullptr, false, onTouchStart);
    emscripten_set_touchend_callback("#canvas", nullptr, false, onTouchEnd);
    emscripten_set_touchmove_callback("#canvas", nullptr, false, onTouchMove);
    emscripten_set_beforeunload_callback(nullptr, onUnload);

    SL_LOG("------------------------------------------------------------------");
    SL_LOG("Platform         : Emscripten");

    AppCommon::calibIniPath = "data/calibrations/";

    SLVstring args;
    slCreateApp(args,
                "data/",
                "data/shaders/",
                "data/models/",
                "data/images/textures/",
                "data/images/fonts/",
                "data/videos/",
                "data/config/",
                "AppDemoEmscripten");

    slLoadCoreAssetsAsync();

    // Request an animation frame from the browser. This will call the `update` function once.
    // The `update` function itself requests the next animation frame, creating an update loop.
    animationFrameID = emscripten_request_animation_frame(onAnimationFrame, nullptr);

    return 0;
}
//-----------------------------------------------------------------------------
//! Paint event handler that passes the event to the slPaintAllViews function.
static SLbool onPaint()
{
    if (AppCommon::sceneViews.empty())
        return false;

    SLSceneView* sv = AppCommon::sceneViews[svIndex];

    int newCanvasWidth  = EM_ASM_INT(return window.devicePixelRatio * window.innerWidth);
    int newCanvasHeight = EM_ASM_INT(return window.devicePixelRatio * window.innerHeight);

    if (newCanvasWidth != canvasWidth || newCanvasHeight != canvasHeight)
    {
        canvasWidth  = newCanvasWidth;
        canvasHeight = newCanvasHeight;
        updateCanvas();

        if (!AppCommon::sceneViews.empty())
            slResize(svIndex,
                     canvasWidth,
                     canvasHeight);
    }

    if (AppCommon::sceneToLoad)
    {
        slSwitchScene(sv, *AppCommon::sceneToLoad);
        AppCommon::sceneToLoad = {}; // sets optional to empty
    }

    if (AppCommon::assetLoader->isLoading())
        AppCommon::assetLoader->checkIfAsyncLoadingIsDone();

    //////////////////////////////////////////////////////////////////////////
    SLbool appNeedsUpdate  = App::config.onUpdate && App::config.onUpdate(sv);
    SLbool jobIsRunning    = slUpdateParallelJob();
    SLbool isLoading       = AppCommon::assetLoader->isLoading();
    SLbool viewNeedsUpdate = slPaintAllViews();
    //////////////////////////////////////////////////////////////////////////

    return appNeedsUpdate || viewNeedsUpdate || jobIsRunning || isLoading;
}
//-----------------------------------------------------------------------------
static void updateCanvas()
{
    // clang-format off
    EM_ASM({
        let canvas = Module['canvas'];
        canvas.width = $0;
        canvas.height = $1;
    }, canvasWidth, canvasHeight);
    // clang-format on
}
//-----------------------------------------------------------------------------
static void onLoadingCoreAssets()
{
    if (AppCommon::assetLoader->isLoading())
    {
        AppCommon::assetLoader->checkIfAsyncLoadingIsDone();
        return;
    }

    coreAssetsLoaded = true;

    slCreateSceneView(AppCommon::assetManager,
                      AppCommon::scene,
                      canvasWidth,
                      canvasHeight,
                      (int)(142.0 * EM_ASM_DOUBLE(return window.devicePixelRatio)),
                      App::config.startSceneID,
                      reinterpret_cast<void*>(onPaint),
                      nullptr,
                      reinterpret_cast<void*>(App::config.onNewSceneView),
                      reinterpret_cast<void*>(App::config.onGuiBuild),
                      reinterpret_cast<void*>(App::config.onGuiLoadConfig),
                      reinterpret_cast<void*>(App::config.onGuiSaveConfig));

    EM_ASM(document.querySelector("#loading-overlay").style.opacity = 0);
}
//-----------------------------------------------------------------------------
static EM_BOOL onAnimationFrame(double time, void* userData)
{
    if (coreAssetsLoaded)
        onPaint();
    else
        onLoadingCoreAssets();

    // Request another animation frame from the browser to run the next iteration of `update`.
    animationFrameID = emscripten_request_animation_frame(onAnimationFrame, nullptr);

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EMSCRIPTEN_RESULT onMousePressed(int                         eventType,
                                        const EmscriptenMouseEvent* mouseEvent,
                                        void*                       userData)
{
    SLint x         = convertToCanvasCoordinate(mouseEvent->targetX);
    SLint y         = convertToCanvasCoordinate(mouseEvent->targetY);
    SLKey modifiers = mapModifiersToSLModifiers(mouseEvent);

    startX = x;
    startY = y;

    switch (mouseEvent->button)
    {
        case 0:
            if (modifiers & K_alt && modifiers & K_ctrl)
                slTouch2Down(svIndex, x - 20, y, x + 20, y);
            else
                slMouseDown(svIndex,
                            MB_left,
                            x,
                            y,
                            modifiers);
            break;
        case 1:
            slMouseDown(svIndex,
                        MB_middle,
                        x,
                        y,
                        modifiers);
            break;
        case 2:
            slMouseDown(svIndex,
                        MB_right,
                        x,
                        y,
                        modifiers);
            break;
        default: break;
    }

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onMouseReleased(int                         eventType,
                               const EmscriptenMouseEvent* mouseEvent,
                               void*                       userData)
{
    SLint x         = convertToCanvasCoordinate(mouseEvent->targetX);
    SLint y         = convertToCanvasCoordinate(mouseEvent->targetY);
    SLKey modifiers = mapModifiersToSLModifiers(mouseEvent);

    startX = -1;
    startY = -1;

    switch (mouseEvent->button)
    {
        case 0:
            slMouseUp(svIndex,
                      MB_left,
                      x,
                      y,
                      modifiers);
            break;
        case 1:
            slMouseUp(svIndex,
                      MB_middle,
                      x,
                      y,
                      modifiers);
            break;
        case 2:
            slMouseUp(svIndex,
                      MB_right,
                      x,
                      y,
                      modifiers);
            break;
        default: break;
    }

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onMouseDoubleClicked(int                         eventType,
                                    const EmscriptenMouseEvent* mouseEvent,
                                    void*                       userData)
{
    SLint x         = convertToCanvasCoordinate(mouseEvent->targetX);
    SLint y         = convertToCanvasCoordinate(mouseEvent->targetY);
    SLKey modifiers = mapModifiersToSLModifiers(mouseEvent);

    switch (mouseEvent->button)
    {
        case 0:
            slDoubleClick(svIndex,
                          MB_left,
                          x,
                          y,
                          modifiers);
            break;
        case 1:
            slDoubleClick(svIndex,
                          MB_middle,
                          x,
                          y,
                          modifiers);
            break;
        case 2:
            slDoubleClick(svIndex,
                          MB_right,
                          x,
                          y,
                          modifiers);
            break;
        default: break;
    }

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onMouseMove(int                         eventType,
                           const EmscriptenMouseEvent* mouseEvent,
                           void*                       userData)
{
    mouseX = convertToCanvasCoordinate(mouseEvent->targetX);
    mouseY = convertToCanvasCoordinate(mouseEvent->targetY);

    if (mouseEvent->altKey && mouseEvent->ctrlKey)
        slTouch2Move(svIndex,
                     mouseX - 20,
                     mouseY,
                     mouseX + 20,
                     mouseY);
    else
        slMouseMove(svIndex,
                    mouseX,
                    mouseY);

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onMouseWheel(int                         eventType,
                            const EmscriptenWheelEvent* wheelEvent,
                            void*                       userData)
{
    // Invert the sign because the scroll value is inverted
    double deltaY = -wheelEvent->deltaY;

    // Make sure the delta is at least one integer
    if (std::abs(deltaY) < 1) deltaY = Utils::sign(wheelEvent->deltaY);

    SLKey modifiers = mapModifiersToSLModifiers(&wheelEvent->mouse);
    slMouseWheel(svIndex, (int)deltaY, modifiers);

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onKeyPressed(int                            eventType,
                            const EmscriptenKeyboardEvent* keyEvent,
                            void*                          userData)
{
    if (keyEvent->repeat)
        return EM_TRUE;

    SLKey key       = mapKeyToSLKey(keyEvent->keyCode);
    SLKey modifiers = mapModifiersToSLModifiers(keyEvent);
    slKeyPress(svIndex, key, modifiers);

    return EM_FALSE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onKeyReleased(int                            eventType,
                             const EmscriptenKeyboardEvent* keyEvent,
                             void*                          userData)
{
    SLKey key       = mapKeyToSLKey(keyEvent->keyCode);
    SLKey modifiers = mapModifiersToSLModifiers(keyEvent);
    slKeyRelease(svIndex, key, modifiers);

    return EM_FALSE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onTouchStart(int                         eventType,
                            const EmscriptenTouchEvent* touchEvent,
                            void*                       userData)
{
    if (touchEvent->numTouches == 1)
    {
        mouseX = convertToCanvasCoordinate(touchEvent->touches[0].clientX);
        mouseY = convertToCanvasCoordinate(touchEvent->touches[0].clientY);
        slMouseDown(svIndex,
                    MB_left,
                    mouseX,
                    mouseY,
                    K_none);
        lastTouchDownTimeMS = touchEvent->timestamp;
    }
    else if (touchEvent->numTouches == 2)
    {
        int x0 = convertToCanvasCoordinate(touchEvent->touches[0].clientX);
        int y0 = convertToCanvasCoordinate(touchEvent->touches[0].clientY);
        int x1 = convertToCanvasCoordinate(touchEvent->touches[1].clientX);
        int y1 = convertToCanvasCoordinate(touchEvent->touches[1].clientY);
        slTouch2Down(svIndex, x0, y0, x1, y1);
    }

    lastTouchDownX = mouseX;
    lastTouchDownY = mouseY;

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onTouchEnd(int                         eventType,
                          const EmscriptenTouchEvent* touchEvent,
                          void*                       userData)
{
    if (touchEvent->numTouches == 1)
    {
        mouseX = convertToCanvasCoordinate(touchEvent->touches[0].clientX);
        mouseY = convertToCanvasCoordinate(touchEvent->touches[0].clientY);
        slMouseUp(svIndex,
                  MB_left,
                  mouseX,
                  mouseY,
                  K_none);

        int    dx = std::abs(mouseX - lastTouchDownX);
        int    dy = std::abs(mouseY - lastTouchDownY);
        double dt = touchEvent->timestamp - lastTouchDownTimeMS;

        if (dt > 800 && dx < 15 && dy < 15)
        {
            slMouseDown(svIndex,
                        MB_right,
                        lastTouchDownX,
                        lastTouchDownY,
                        K_none);
            slMouseUp(svIndex,
                      MB_right,
                      lastTouchDownX,
                      lastTouchDownY,
                      K_none);
        }
    }
    else if (touchEvent->numTouches == 2)
    {
        int x0 = convertToCanvasCoordinate(touchEvent->touches[0].clientX);
        int y0 = convertToCanvasCoordinate(touchEvent->touches[0].clientY);
        int x1 = convertToCanvasCoordinate(touchEvent->touches[1].clientX);
        int y1 = convertToCanvasCoordinate(touchEvent->touches[1].clientY);
        slTouch2Up(svIndex, x0, y0, x1, y1);
    }

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static EM_BOOL onTouchMove(int                         eventType,
                           const EmscriptenTouchEvent* touchEvent,
                           void*                       userData)
{
    if (touchEvent->numTouches == 1)
    {
        mouseX = convertToCanvasCoordinate(touchEvent->touches[0].clientX);
        mouseY = convertToCanvasCoordinate(touchEvent->touches[0].clientY);
        slMouseMove(svIndex, mouseX, mouseY);
    }
    else if (touchEvent->numTouches == 2)
    {
        int x0 = convertToCanvasCoordinate(touchEvent->touches[0].clientX);
        int y0 = convertToCanvasCoordinate(touchEvent->touches[0].clientY);
        int x1 = convertToCanvasCoordinate(touchEvent->touches[1].clientX);
        int y1 = convertToCanvasCoordinate(touchEvent->touches[1].clientY);
        slTouch2Move(svIndex, x0, y0, x1, y1);
    }

    return EM_TRUE;
}
//-----------------------------------------------------------------------------
static const char* onUnload(int         eventType,
                            const void* reserved,
                            void*       userData)
{
    slTerminate();

    // Cancel the current animation frame to prevent `update` being called after
    // everything has been terminated and cleaned up.
    emscripten_cancel_animation_frame(animationFrameID);

    return nullptr;
}
//-----------------------------------------------------------------------------
static SLint convertToCanvasCoordinate(SLint coordinate)
{
    return (SLint)((double)coordinate * EM_ASM_DOUBLE(return window.devicePixelRatio));
}
//-----------------------------------------------------------------------------
static SLKey mapKeyToSLKey(unsigned long key)
{
    switch (key)
    {
        case 8: return K_backspace;
        case 9: return K_tab;
        case 13: return K_enter;
        case 16: return K_shift;
        case 17: return K_ctrl;
        case 18: return K_alt;
        case 27: return K_esc;
        case 32: return K_space;
        case 33: return K_pageUp;
        case 34: return K_pageDown;
        case 35: return K_end;
        case 36: return K_home;
        case 37: return K_left;
        case 38: return K_up;
        case 39: return K_right;
        case 40: return K_down;
        case 45: return K_insert;
        case 46: return K_delete;
        case 96: return K_NP0;
        case 97: return K_NP1;
        case 98: return K_NP2;
        case 99: return K_NP3;
        case 100: return K_NP4;
        case 101: return K_NP5;
        case 102: return K_NP6;
        case 103: return K_NP7;
        case 104: return K_NP8;
        case 105: return K_NP9;
        case 106: return K_NPMultiply;
        case 107: return K_NPAdd;
        case 109: return K_NPSubtract;
        case 110: return K_NPDecimal;
        case 111: return K_NPDivide;
        case 112: return K_F1;
        case 113: return K_F2;
        case 114: return K_F3;
        case 115: return K_F4;
        case 116: return K_F5;
        case 117: return K_F6;
        case 118: return K_F7;
        case 119: return K_F8;
        case 120: return K_F9;
        case 121: return K_F10;
        case 122: return K_F11;
        case 123: return K_F12;
        default: return (SLKey)key;
    }
}
//-----------------------------------------------------------------------------
SLKey mapModifiersToSLModifiers(bool shiftDown, bool ctrlDown, bool altDown)
{
    int modifiers = 0;
    if (shiftDown) modifiers |= K_shift;
    if (ctrlDown) modifiers |= K_ctrl;
    if (altDown) modifiers |= K_alt;
    return (SLKey)modifiers;
}
//-----------------------------------------------------------------------------
SLKey mapModifiersToSLModifiers(const EmscriptenMouseEvent* mouseEvent)
{
    return mapModifiersToSLModifiers(mouseEvent->shiftKey,
                                     mouseEvent->ctrlKey,
                                     mouseEvent->altKey);
}
//-----------------------------------------------------------------------------
SLKey mapModifiersToSLModifiers(const EmscriptenKeyboardEvent* keyEvent)
{
    return mapModifiersToSLModifiers(keyEvent->shiftKey,
                                     keyEvent->ctrlKey,
                                     keyEvent->altKey);
}
//-----------------------------------------------------------------------------