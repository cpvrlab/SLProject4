#ifndef APPDEMOSCENEEMPTY_H
#define APPDEMOSCENEEMPTY_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneEmpty : public AppScene
{
public:
    AppDemoSceneEmpty();
    void recordAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
