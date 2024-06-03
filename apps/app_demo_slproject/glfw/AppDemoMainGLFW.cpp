//#############################################################################
//  File:      AppDemoMainGLFW.cpp
//  Purpose:   The demo application demonstrates most features of the SLProject
//             framework. Implementation of the GUI with the GLFW3 framework
//             that can create a window and receive system event on desktop OS
//             such as Windows, MacOS and Linux.
//  Date:      July 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <SLGLState.h>
#include <SLEnums.h>
#include <SLInterface.h>
#include <AppDemo.h>
#include <SLAssetManager.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <CVCapture.h>
#include <AppDemoGui.h>
#include <AppDemoSceneView.h>
#include <Profiler.h>
#include <SLAssetLoader.h>
#include <GLFW/glfw3.h>

//-----------------------------------------------------------------------------
bool onUpdateVideo();
//-----------------------------------------------------------------------------
// Global application variables
static GLFWwindow* window;                     //!< The global glfw window handle
static SLint       svIndex;                    //!< SceneView index
static SLint       scrWidth;                   //!< Window width at start up
static SLint       scrHeight;                  //!< Window height at start up
static SLbool      fixAspectRatio;             //!< Flag if wnd aspect ratio should be fixed
static SLfloat     scrWdivH;                   //!< aspect ratio screen width divided by height
static SLint       dpi = 142;                  //!< Dot per inch resolution of screen
static SLint       startX;                     //!< start position x in pixels
static SLint       startY;                     //!< start position y in pixels
static SLint       mouseX;                     //!< Last mouse position x in pixels
static SLint       mouseY;                     //!< Last mouse position y in pixels
static SLVec2i     touch2;                     //!< Last finger touch 2 position in pixels
static SLVec2i     touchDelta;                 //!< Delta between two fingers in x
static SLint       lastWidth;                  //!< Last window width in pixels
static SLint       lastHeight;                 //!< Last window height in pixels
static SLfloat     lastMouseDownTime = 0.0f;   //!< Last mouse press time
static SLKey       modifiers         = K_none; //!< last modifier keys
static SLbool      fullscreen        = false;  //!< flag if window is in fullscreen mode
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//! Maps the GLFW key codes to the SLKey codes
SLKey mapKeyToSLKey(SLint key)
{
    switch (key)
    {
        case GLFW_KEY_SPACE: return K_space;
        case GLFW_KEY_ESCAPE: return K_esc;
        case GLFW_KEY_F1: return K_F1;
        case GLFW_KEY_F2: return K_F2;
        case GLFW_KEY_F3: return K_F3;
        case GLFW_KEY_F4: return K_F4;
        case GLFW_KEY_F5: return K_F5;
        case GLFW_KEY_F6: return K_F6;
        case GLFW_KEY_F7: return K_F7;
        case GLFW_KEY_F8: return K_F8;
        case GLFW_KEY_F9: return K_F9;
        case GLFW_KEY_F10: return K_F10;
        case GLFW_KEY_F11: return K_F11;
        case GLFW_KEY_F12: return K_F12;
        case GLFW_KEY_UP: return K_up;
        case GLFW_KEY_DOWN: return K_down;
        case GLFW_KEY_LEFT: return K_left;
        case GLFW_KEY_RIGHT: return K_right;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT: return K_shift;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL: return K_ctrl;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT: return K_alt;
        case GLFW_KEY_LEFT_SUPER:
        case GLFW_KEY_RIGHT_SUPER: return K_super; // Apple command key
        case GLFW_KEY_TAB: return K_tab;
        case GLFW_KEY_ENTER: return K_enter;
        case GLFW_KEY_BACKSPACE: return K_backspace;
        case GLFW_KEY_INSERT: return K_insert;
        case GLFW_KEY_DELETE: return K_delete;
        case GLFW_KEY_PAGE_UP: return K_pageUp;
        case GLFW_KEY_PAGE_DOWN: return K_pageDown;
        case GLFW_KEY_HOME: return K_home;
        case GLFW_KEY_END: return K_end;
        case GLFW_KEY_KP_0: return K_NP0;
        case GLFW_KEY_KP_1: return K_NP1;
        case GLFW_KEY_KP_2: return K_NP2;
        case GLFW_KEY_KP_3: return K_NP3;
        case GLFW_KEY_KP_4: return K_NP4;
        case GLFW_KEY_KP_5: return K_NP5;
        case GLFW_KEY_KP_6: return K_NP6;
        case GLFW_KEY_KP_7: return K_NP7;
        case GLFW_KEY_KP_8: return K_NP8;
        case GLFW_KEY_KP_9: return K_NP9;
        case GLFW_KEY_KP_DIVIDE: return K_NPDivide;
        case GLFW_KEY_KP_MULTIPLY: return K_NPMultiply;
        case GLFW_KEY_KP_SUBTRACT: return K_NPSubtract;
        case GLFW_KEY_KP_ADD: return K_NPAdd;
        case GLFW_KEY_KP_DECIMAL: return K_NPDecimal;
        case GLFW_KEY_UNKNOWN: return K_none;
        default: break;
    }
    return (SLKey)key;
}
//-----------------------------------------------------------------------------
/*!
onPaint: Paint event handler that passes the event to the slPaint function.
*/
SLbool onPaint()
{
    PROFILE_SCOPE("GLFW::onPaint");

    if (AppDemo::sceneViews.empty())
        return false;

    SLSceneView* sv = AppDemo::sceneViews[svIndex];

    if (AppDemo::sceneToLoad)
    {
        slSwitchScene(sv, *AppDemo::sceneToLoad);
        AppDemo::sceneToLoad = {}; // sets optional to false
    }

    if (AppDemo::assetLoader->isLoading())
        AppDemo::assetLoader->checkIfAsyncLoadingIsDone();

    // If live video image is requested grab it and copy it
    if (CVCapture::instance()->videoType() != VT_NONE)
    {
        float viewportWdivH = sv->viewportWdivH();
        CVCapture::instance()->grabAndAdjustForSL(viewportWdivH);
    }

    ////////////////////////////////////////////////
    bool trackingGotUpdated = onUpdateVideo();
    bool jobIsRunning       = slUpdateParallelJob();
    bool viewsNeedsRepaint  = slPaintAllViews();
    ////////////////////////////////////////////////

    // Fast copy the back buffer to the front buffer. This is OS dependent.
    glfwSwapBuffers(window);

    // Show the title generated by the scene library (FPS etc.)
    glfwSetWindowTitle(window, sv->windowTitle().c_str());

    return trackingGotUpdated ||
           jobIsRunning ||
           viewsNeedsRepaint ||
           AppDemo::assetLoader->isLoading();
}
//-----------------------------------------------------------------------------
/*!
onResize: Event handler called on the resize event of the window. This event
should called once before the onPaint event.
*/
static void onResize(GLFWwindow* myWindow, int width, int height)
{
    if (AppDemo::sceneViews.empty()) return;
    SLSceneView* sv = AppDemo::sceneViews[svIndex];

    if (fixAspectRatio)
    {
        // correct target width and height
        if ((float)height * scrWdivH <= (float)width)
        {
            width  = (int)((float)height * scrWdivH);
            height = (int)((float)width / scrWdivH);
        }
        else
        {
            height = (int)((float)width / scrWdivH);
            width  = (int)((float)height * scrWdivH);
        }
    }

    lastWidth  = width;
    lastHeight = height;

    // width & height are in screen coords.
    slResize(svIndex, width, height);

    onPaint();
}
//-----------------------------------------------------------------------------
/*!
Mouse button event handler forwards the events to the slMouseDown or slMouseUp.
Two finger touches of touch devices are simulated with ALT & CTRL modifiers.
*/
static void onMouseButton(GLFWwindow* myWindow,
                          int         button,
                          int         action,
                          int         mods)
{
    SLint x = mouseX;
    SLint y = mouseY;
    startX  = x;
    startY  = y;

    // Translate modifiers
    modifiers = K_none;
    if ((uint)mods & (uint)GLFW_MOD_SHIFT) modifiers = (SLKey)(modifiers | K_shift);
    if ((uint)mods & (uint)GLFW_MOD_CONTROL) modifiers = (SLKey)(modifiers | K_ctrl);
    if ((uint)mods & (uint)GLFW_MOD_ALT) modifiers = (SLKey)(modifiers | K_alt);

    if (action == GLFW_PRESS)
    {
        SLfloat mouseDeltaTime = (SLfloat)glfwGetTime() - lastMouseDownTime;
        lastMouseDownTime      = (SLfloat)glfwGetTime();

        // handle double click
        if (mouseDeltaTime < 0.3f)
        {
            switch (button)
            {
                case GLFW_MOUSE_BUTTON_LEFT:
                    slDoubleClick(svIndex, MB_left, x, y, modifiers);
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    slDoubleClick(svIndex, MB_right, x, y, modifiers);
                    break;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    slDoubleClick(svIndex, MB_middle, x, y, modifiers);
                    break;
                default: break;
            }
        }
        else // normal mouse clicks
        {
            switch (button)
            {
                case GLFW_MOUSE_BUTTON_LEFT:
                    if (modifiers & K_alt && modifiers & K_ctrl)
                        slTouch2Down(svIndex, x - 20, y, x + 20, y);
                    else
                        slMouseDown(svIndex, MB_left, x, y, modifiers);
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    slMouseDown(svIndex, MB_right, x, y, modifiers);
                    break;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                    slMouseDown(svIndex, MB_middle, x, y, modifiers);
                    break;
                default: break;
            }
        }
    }
    else
    { // flag end of mouse click for long touches
        startX = -1;
        startY = -1;

        switch (button)
        {
            case GLFW_MOUSE_BUTTON_LEFT:
                slMouseUp(svIndex, MB_left, x, y, modifiers);
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                slMouseUp(svIndex, MB_right, x, y, modifiers);
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                slMouseUp(svIndex, MB_middle, x, y, modifiers);
                break;
            default: break;
        }
    }
}
//-----------------------------------------------------------------------------
/*!
Mouse move event handler forwards the events to slMouseMove or slTouch2Move.
*/
static void onMouseMove(GLFWwindow* myWindow,
                        double      x,
                        double      y)
{
    // x & y are in screen coords.
    mouseX = (int)x;
    mouseY = (int)y;

    if (modifiers & K_alt && modifiers & K_ctrl)
        slTouch2Move(svIndex, (int)(x - 20), (int)y, (int)(x + 20), (int)y);
    else
        slMouseMove(svIndex, (int)x, (int)y);
}
//-----------------------------------------------------------------------------
/*!
Mouse wheel event handler forwards the events to slMouseWheel
*/
static void onMouseWheel(GLFWwindow* myWindow,
                         double      xscroll,
                         double      yscroll)
{
    // make sure the delta is at least one integer
    int dY = (int)yscroll;
    if (dY == 0) dY = (int)(Utils::sign(yscroll));

    slMouseWheel(svIndex, dY, modifiers);
}
//-----------------------------------------------------------------------------
/*!
Key event handler sets the modifier key state & forwards the event to
the slKeyPress function.
*/
static void onKeyPress(GLFWwindow* myWindow,
                       int         GLFWKey,
                       int         scancode,
                       int         action,
                       int         mods)
{
    SLKey key = mapKeyToSLKey(GLFWKey);

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case K_ctrl: modifiers = (SLKey)(modifiers | K_ctrl); return;
            case K_alt: modifiers = (SLKey)(modifiers | K_alt); return;
            case K_shift: modifiers = (SLKey)(modifiers | K_shift); return;
            default: break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
            case K_ctrl: modifiers = (SLKey)(modifiers ^ K_ctrl); return;
            case K_alt: modifiers = (SLKey)(modifiers ^ K_alt); return;
            case K_shift: modifiers = (SLKey)(modifiers ^ K_shift); return;
            default: break;
        }
    }

    // Special treatment for ESC key
    if (key == K_esc && action == GLFW_RELEASE)
    {
        if (fullscreen)
        {
            fullscreen = !fullscreen;
            glfwSetWindowSize(myWindow, scrWidth, scrHeight);
            glfwSetWindowPos(myWindow, 10, 30);
        }
        if (AppDemoGui::hideUI)
            AppDemoGui::hideUI = false;
    }
    // Toggle fullscreen mode
    else if (key == K_F9 && action == GLFW_PRESS)
    {
        fullscreen = !fullscreen;

        if (fullscreen)
        {
            GLFWmonitor*       primary = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode    = glfwGetVideoMode(primary);
            glfwSetWindowSize(myWindow, mode->width, mode->height);
            glfwSetWindowPos(myWindow, 0, 0);
        }
        else
        {
            glfwSetWindowSize(myWindow, scrWidth, scrHeight);
            glfwSetWindowPos(myWindow, 10, 30);
        }
    }
    else
    {
        // Keyboard shortcuts for next or previous sceneID loading
        if (modifiers & K_alt && modifiers & K_shift)
        {
            SLSceneView* sv = AppDemo::sceneViews[0];
            if (action == GLFW_PRESS)
            {
                if (key == '0' && sv)
                {
                    AppDemo::sceneToLoad = SID_Empty;
                    SL_LOG("----------------------------------------------");
                    SL_LOG("Loading SceneID: %d", AppDemo::sceneID);
                }
                else if (key == K_left && sv && AppDemo::sceneID > 0)
                {
                    AppDemo::sceneToLoad = static_cast<SLSceneID>(AppDemo::sceneID - 1);
                    SL_LOG("----------------------------------------------");
                    SL_LOG("Loading SceneID: %d", AppDemo::sceneID);
                }
                else if (key == K_right && sv && AppDemo::sceneID < SID_MaxNoBenchmarks - 1)
                {
                    AppDemo::sceneToLoad = static_cast<SLSceneID>(AppDemo::sceneID + 1);
                    SL_LOG("----------------------------------------------");
                    SL_LOG("Loading SceneID: %d", AppDemo::sceneID);
                }
            }
            return;
        }

        if (action == GLFW_PRESS)
            slKeyPress(svIndex, key, modifiers);
        else if (action == GLFW_RELEASE)
            slKeyRelease(svIndex, key, modifiers);
    }
}
//-----------------------------------------------------------------------------
//! Event handler for GLFW character input
void onCharInput(GLFWwindow*, SLuint c)
{
    slCharInput(svIndex, c);
}
//-----------------------------------------------------------------------------
/*!
onClose event handler for deallocation of the scene & sceneview. onClose is
called glfwPollEvents, glfwWaitEvents or glfwSwapBuffers.
*/
void onClose(GLFWwindow* myWindow)
{
    slShouldClose(true);
}
//-----------------------------------------------------------------------------
/*!
Error callback handler for GLFW.
*/
void onGLFWError(int error, const char* description)
{
    fputs(description, stderr);
}
//-----------------------------------------------------------------------------
//! Alternative SceneView creation C-function passed by slCreateSceneView
SLSceneView* createAppDemoSceneView(SLScene*        scene,
                                    int             curDPI,
                                    SLInputManager& inputManager)
{
    // The sceneview will be deleted by SLScene::~SLScene()
    return new AppDemoSceneView(scene, curDPI, inputManager);
}
//-----------------------------------------------------------------------------
//! Initialises all GLFW and GL3W stuff
void initGLFW(int screenWidth, int screenHeight)
{
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(onGLFWError);

    // Enable fullscreen anti aliasing with 4 samples
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    // You can enable or restrict newer OpenGL context here (read the GLFW documentation)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
#endif

    window = glfwCreateWindow(screenWidth, screenHeight, "My Title", nullptr, nullptr);

    // get real window size
    glfwGetWindowSize(window, &scrWidth, &scrHeight);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Get the current GL context. After this you can call GL
    glfwMakeContextCurrent(window);

    // Init OpenGL access library gl3w
    if (gl3wInit() != 0)
    {
        cerr << "Failed to initialize OpenGL" << endl;
        exit(-1);
    }

    glfwSetWindowTitle(window, "SLProject Test Application");
    glfwSetWindowPos(window, 50, 100);

    // With GLFW ImGui draws the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Set number of monitor refreshes between 2 buffer swaps
    glfwSwapInterval(1);

    // Get GL errors that occurred before our framework is involved
    GET_GL_ERROR;

    // Set your own physical screen dpi
    Utils::log("SLProject", "------------------------------------------------------------------");
    Utils::log("SLProject",
               "GUI-Framwork     : GLFW (Version: %d.%d.%d",
               GLFW_VERSION_MAJOR,
               GLFW_VERSION_MINOR,
               GLFW_VERSION_REVISION);
    Utils::log("SLProject",
               "Resolution (DPI) : %d",
               dpi);

    // Set GLFW callback functions
    glfwSetKeyCallback(window, onKeyPress);
    glfwSetCharCallback(window, onCharInput);
    glfwSetWindowSizeCallback(window, onResize);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseWheel);
    glfwSetWindowCloseCallback(window, onClose);
}
//-----------------------------------------------------------------------------
//! Inits all for SLProject library
void initSL(SLVstring& cmdLineArgs)
{
    // get executable path
    SLstring projectRoot = SLstring(SL_PROJECT_ROOT);
    SLstring configDir   = Utils::getAppsWritableDir();
    slSetupExternalDir(projectRoot + "/data/");
    // Utils::dumpFileSystemRec("SLProject",  projectRoot + "/data");

    // setup platform dependent data path
    AppDemo::calibFilePath = configDir;
    AppDemo::calibIniPath  = projectRoot + "/data/calibrations/";
    CVCapture::instance()->loadCalibrations(Utils::ComputerInfos::get(),
                                            AppDemo::calibFilePath);

    /////////////////////////////////////////////////////////
    slCreateApp(cmdLineArgs,
                projectRoot + "/data/",
                projectRoot + "/data/shaders/",
                projectRoot + "/data/models/",
                projectRoot + "/data/images/textures/",
                projectRoot + "/data/images/fonts/",
                projectRoot + "/data/videos/",
                configDir,
                "AppDemoGLFW");
    /////////////////////////////////////////////////////////

    slLoadCoreAssetsSync();

    /////////////////////////////////////////////////////////
    slCreateSceneView(AppDemo::assetManager,
                      AppDemo::scene,
                      scrWidth,
                      scrHeight,
                      dpi,
                      (SLSceneID)SL_STARTSCENE,
                      (void*)&onPaint,
                      nullptr,
                      (void*)createAppDemoSceneView,
                      (void*)AppDemoGui::build,
                      (void*)AppDemoGui::loadConfig,
                      (void*)AppDemoGui::saveConfig);
    /////////////////////////////////////////////////////////
}
//-----------------------------------------------------------------------------
/*!
The C main procedure running the GLFW GUI application.
*/
int main(int argc, char* argv[])
{
    // set command line arguments
    SLVstring cmdLineArgs;
    for (int i = 0; i < argc; i++)
        cmdLineArgs.push_back(SLstring(argv[i]));

    // parse cmd line arguments
    for (int i = 1; i < cmdLineArgs.size(); ++i)
    {
        if (cmdLineArgs[i] == "-onlyErrorLogs")
        {
            Utils::onlyErrorLogs = true;
        }
    }

    scrWidth       = 1280;
    scrHeight      = 720;
    scrWdivH       = (float)scrWidth / (float)scrHeight;
    fixAspectRatio = false;
    touch2.set(-1, -1);
    touchDelta.set(-1, -1);

    initGLFW(scrWidth, scrHeight);
    initSL(cmdLineArgs);

    // Event loop
    while (!slShouldClose())
    {
        /////////////////////////////
        SLbool doRepaint = onPaint();
        /////////////////////////////

        // if no updated occurred wait for the next event (power saving)
        if (!doRepaint)
            // todo ghm1: glfwWaitEvents is not working on my machine (maybe https://github.com/glfw/glfw/issues/685)
            glfwWaitEvents();
        else
            glfwPollEvents();
    }

    slTerminate();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
//-----------------------------------------------------------------------------
