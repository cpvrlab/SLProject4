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
void AppDemoSceneLegacy::recordAssetsToLoad(SLAssetManager* am)
{
}
//-----------------------------------------------------------------------------
void AppDemoSceneLegacy::assemble(SLAssetManager* am, SLSceneView* sv)
{
    appDemoLoadScene(am, this, sv, _sceneID);
}
//-----------------------------------------------------------------------------
