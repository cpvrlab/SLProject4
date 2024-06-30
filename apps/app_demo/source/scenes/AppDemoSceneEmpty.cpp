/**
 * \file      AppDemoSceneEmpty.cpp
 * \brief     Implementation for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppDemoSceneEmpty.h>
#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppCommon.h>

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
