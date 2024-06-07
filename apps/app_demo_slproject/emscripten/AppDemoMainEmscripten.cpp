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
#include <AppDemo.h>
#include <CVCapture.h>
#include <AppDemoGui.h>
#include <AppDemoSceneView.h>
#include <App.h>

//-----------------------------------------------------------------------------
bool onUpdateVideo();

//-----------------------------------------------------------------------------
static SLbool onUpdate(SLSceneView* sv)
{
    // If live video image is requested grab it and copy it
    if (CVCapture::instance()->videoType() != VT_NONE)
    {
        float viewportWdivH = sv->viewportWdivH();
        CVCapture::instance()->grabAndAdjustForSL(viewportWdivH);
    }

    SLbool trackingGotUpdated = onUpdateVideo();
    return trackingGotUpdated;
}
//-----------------------------------------------------------------------------
static SLSceneView* createSceneView(SLScene*        scene,
                                    int             curDPI,
                                    SLInputManager& inputManager)
{
    // The sceneview will be deleted by SLScene::~SLScene()
    return new AppDemoSceneView(scene, curDPI, inputManager);
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    return App::run({.argc            = argc,
                     .argv            = argv,
                     .windowTitle     = "SLProject Test Application",
                     .windowWidth     = 1280,
                     .windowHeight    = 720,
                     .numSamples      = 4,
                     .startSceneID    = SL_STARTSCENE,
                     .onNewSceneView  = createSceneView,
                     .onUpdate        = onUpdate,
                     .onGuiBuild      = AppDemoGui::build,
                     .onGuiLoadConfig = AppDemoGui::loadConfig,
                     .onGuiSaveConfig = AppDemoGui::saveConfig});
}
//-----------------------------------------------------------------------------
