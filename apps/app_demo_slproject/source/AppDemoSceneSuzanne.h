#ifndef APPDEMOSCENESUZANNE_H
#define APPDEMOSCENESUZANNE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneSuzanne : public AppScene
{
public:
    AppDemoSceneSuzanne(SLstring name,
                        bool     textureMapping,
                        bool     normalMapping,
                        bool     occlusionMapping,
                        bool     shadowMapping);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _suzanneInCube;
    bool    _textureMapping;
    bool    _normalMapping;
    bool    _occlusionMapping;
    bool    _shadowMapping;
};
//-----------------------------------------------------------------------------

#endif
