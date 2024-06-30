/**
 * \file      AppDemoSceneTextureCompression.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENETEXTURECOMPRESSION_H
#define APPDEMOSCENETEXTURECOMPRESSION_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for texture compression test scene
class AppDemoSceneTextureCompression : public SLScene
{
public:
    AppDemoSceneTextureCompression();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texPng;
    SLGLTexture* _texJpgQ90;
    SLGLTexture* _texJpgQ40;
    SLGLTexture* _texKtxBcmp255;
    SLGLTexture* _texKtxBcmp128;
    SLGLTexture* _texKtxBcmp001;
    SLGLTexture* _texKtxUastc4;
    SLGLTexture* _texKtxUastc2;
    SLGLTexture* _texKtxUastc0;
};
//-----------------------------------------------------------------------------

#endif
