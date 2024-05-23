
#ifndef APPDEMOSCENELEGACY_H
#define APPDEMOSCENELEGACY_H

#include <SLEnums.h>
#include <AppScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneLegacy : public AppScene
{
public:
    AppDemoSceneLegacy(SLSceneID sceneID);
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSceneID _sceneID;
};
//-----------------------------------------------------------------------------

#endif
