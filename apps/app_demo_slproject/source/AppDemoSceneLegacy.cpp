#include <AppDemoSceneLegacy.h>

#include <SLLightSpot.h>
#include <SLRectangle.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
void appDemoLoadScene(SLAssetManager* am,
                      SLScene*        s,
                      SLSceneView*    sv,
                      SLSceneID       sceneID);
//-----------------------------------------------------------------------------
AppDemoSceneLegacy::AppDemoSceneLegacy(SLSceneID sceneID)
  : AppScene("Dummy"),
    _sceneID(sceneID)
{
}
//-----------------------------------------------------------------------------
//! All assets the should be loaded in parallel must be registered in here.
void AppDemoSceneLegacy::registerAssetsToLoad(SLAssetLoader& al)
{
}
//-----------------------------------------------------------------------------
//! After parallel loading of the assets the scene gets assembled in here.
void AppDemoSceneLegacy::assemble(SLAssetManager* am, SLSceneView* sv)
{
    appDemoLoadScene(am, this, sv, _sceneID);
}
//-----------------------------------------------------------------------------
