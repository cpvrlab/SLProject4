//#############################################################################
//  File:      AppDemoSceneShaderCook.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERCOOK_H
#define APPDEMOSCENESHADERCOOK_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for Cook-Torrance lighting demo scene
class AppDemoSceneShaderCook : public SLScene
{
public:
    AppDemoSceneShaderCook();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLProgram* _sp;
    SLGLProgram* _spTex;
    SLGLTexture* _texC;
    SLGLTexture* _texN;
    SLGLTexture* _texM;
    SLGLTexture* _texR;
};
//-----------------------------------------------------------------------------

#endif
