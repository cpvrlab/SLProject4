#ifndef APPSCENE_H
#define APPSCENE_H

#include <SLScene.h>
#include <SLAssetLoader.h>
#include <SLSceneView.h>
#include <SLAssetManager.h>

//-----------------------------------------------------------------------------
class AppScene : public SLScene
{
public:
    AppScene(SLstring name) : SLScene(name) {}
    virtual void recordAssetsToLoad(SLAssetLoader& al)         = 0;
    virtual void assemble(SLAssetManager* am, SLSceneView* sv) = 0;
};
//-----------------------------------------------------------------------------

#endif
