//#############################################################################
//  File:      AppDemoSceneTextureFilter.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENETEXTUREFILTER_H
#define APPDEMOSCENETEXTUREFILTER_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
// Class for texture filtering scene
class AppDemoSceneTextureFilter : public AppScene
{
public:
    AppDemoSceneTextureFilter();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texB;
    SLGLTexture* _texL;
    SLGLTexture* _texT;
    SLGLTexture* _texR;
    SLGLTexture* _tex3D;
    SLGLProgram* _spr3D;
};
//-----------------------------------------------------------------------------

#endif
