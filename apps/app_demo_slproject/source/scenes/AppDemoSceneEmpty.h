#ifndef APPDEMOSCENEEMPTY_H
#define APPDEMOSCENEEMPTY_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneEmpty : public SLScene
{
public:
    AppDemoSceneEmpty();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
