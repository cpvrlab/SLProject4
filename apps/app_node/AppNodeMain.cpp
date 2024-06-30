/**
 * \file      AppNodeMain.cpp
 * \brief     File with the main function and two callbacks for the node demo app
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <App.h>
#include <AppCommon.h>
#include <AppNodeGui.h>
#include <AppNodeSceneView.h>
#include <AppNodeScene.h>

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
    App::Config config;
    config.argc           = argc;
    config.argv           = argv;
    config.windowWidth    = 640;
    config.windowHeight   = 480;
    config.windowTitle    = "SLProject Node Demo";
    config.onNewSceneView = createSceneView;
    config.onNewScene     = createScene;
    config.onGuiBuild     = AppNodeGui::build;
    
    return App::run(config);
}
//-----------------------------------------------------------------------------
