//#############################################################################
//  File:      AppDemoSceneTextureCompression.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENETEXTURECOMPRESSION_H
#define APPDEMOSCENETEXTURECOMPRESSION_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for texture compression test scene
class AppDemoSceneTextureCompression : public AppScene
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
