#include <AppDemoSceneEmpty.h>

#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
AppDemoSceneEmpty::AppDemoSceneEmpty() : AppScene("Empty Scene")
{
    info("No Scene loaded.");
}
//-----------------------------------------------------------------------------
void AppDemoSceneEmpty::recordAssetsToLoad(SLAssetManager* am)
{
}
//-----------------------------------------------------------------------------
void AppDemoSceneEmpty::assemble(SLAssetManager* am, SLSceneView* sv)
{
    root3D(nullptr);

    sv->sceneViewCamera()->background().colors(SLCol4f(0.7f, 0.7f, 0.7f),
                                               SLCol4f(0.2f, 0.2f, 0.2f));
    sv->camera(nullptr);
    sv->doWaitOnIdle(true);
}
//-----------------------------------------------------------------------------
