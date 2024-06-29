/**
 * \file      AppDemoSceneSuzanne.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENESUZANNE_H
#define APPDEMOSCENESUZANNE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for all variants of generated shaders on the Suzanne head
class AppDemoSceneSuzanne : public SLScene
{
public:
    AppDemoSceneSuzanne(SLstring name,
                        bool     textureMapping,
                        bool     normalMapping,
                        bool     occlusionMapping,
                        bool     shadowMapping,
                        bool     environmentMapping);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*   _suzanneInCube;
    SLSkybox* _skybox;
    bool      _textureMapping;
    bool      _normalMapping;
    bool      _occlusionMapping;
    bool      _shadowMapping;
    bool      _environmentMapping;
};
//-----------------------------------------------------------------------------

#endif
