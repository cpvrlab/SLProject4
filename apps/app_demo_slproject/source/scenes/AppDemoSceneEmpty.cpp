//#############################################################################
//  File:      AppDemoSceneEmpty.cpp
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppDemoSceneEmpty.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneEmpty::AppDemoSceneEmpty() : SLScene("Empty Scene")
{
    info("No Scene loaded.");
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneEmpty::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneEmpty::assemble(SLAssetManager* am, SLSceneView* sv)
{
    root3D(nullptr);

    sv->sceneViewCamera()->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    sv->camera(nullptr);
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
