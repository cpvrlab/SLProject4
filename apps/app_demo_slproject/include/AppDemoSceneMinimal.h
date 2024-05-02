#ifndef APPDEMOSCENEMINIMAL_H
#define APPDEMOSCENEMINIMAL_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneMinimal : public AppScene
{
public:
    AppDemoSceneMinimal();
    void recordAssetsToLoad(SLAssetManager* am) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
};
//-----------------------------------------------------------------------------

#endif
