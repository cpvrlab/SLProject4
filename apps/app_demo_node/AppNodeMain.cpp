//#############################################################################
//  File:      AppNodeMain.cpp
//  Date:      July 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemo.h>
#include <AppNodeGui.h>
#include <AppNodeSceneView.h>
#include <AppNodeScene.h>
#include <App.h>

//-----------------------------------------------------------------------------
static SLSceneView* createSceneView(SLScene*        scene,
                                    int             myDPI,
                                    SLInputManager& inputManager)
{
    return new AppNodeSceneView(scene, myDPI, inputManager);
}
//-----------------------------------------------------------------------------
static SLScene* createScene(SLSceneID sceneID)
{
    return new AppNodeScene();
}
//-----------------------------------------------------------------------------
int SL_MAIN_FUNCTION(int argc, char* argv[])
{
    return App::run({.argc           = argc,
                     .argv           = argv,
                     .windowTitle    = "SLProject Node Demo",
                     .windowWidth    = 640,
                     .windowHeight   = 480,
                     .onNewSceneView = createSceneView,
                     .onNewScene     = createScene,
                     .onGuiBuild     = AppNodeGui::build});
}
//-----------------------------------------------------------------------------
