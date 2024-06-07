//#############################################################################
//  File:      AppNodeMain.cpp
//  Date:      July 2014
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemo.h>
#include <App.h>
#include <AppNodeGui.h>
#include <AppNodeSceneView.h>

//-----------------------------------------------------------------------------
SLSceneView* createSceneView(SLScene*        scene,
                             int             myDPI,
                             SLInputManager& inputManager)
{
    return new AppNodeSceneView(scene, myDPI, inputManager);
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    return App::run({.argc           = argc,
                     .argv           = argv,
                     .windowTitle    = "SLProject Node Demo",
                     .windowWidth    = 640,
                     .windowHeight   = 480,
                     .onNewSceneView = createSceneView,
                     .onGuiBuild     = AppNodeGui::build});
}
//-----------------------------------------------------------------------------
