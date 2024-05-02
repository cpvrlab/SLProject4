#ifndef APPSCENE_H
#define APPSCENE_H

#include <SLScene.h>
#include <SLAssetManager.h>
#include <SLSceneView.h>

//-----------------------------------------------------------------------------
class AppScene : public SLScene
{
public:
    AppScene(SLstring name) : SLScene(name) {}
    virtual void recordAssetsToLoad(SLAssetManager* am)        = 0;
    virtual void assemble(SLAssetManager* am, SLSceneView* sv) = 0;
};
//-----------------------------------------------------------------------------

#endif
