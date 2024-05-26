//#############################################################################
//  File:      AppDemoSceneShaderIBL.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERIBL_H
#define APPDEMOSCENESHADERIBL_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for image base lighting demo scene
class AppDemoSceneShaderIBL : public SLScene
{
public:
    AppDemoSceneShaderIBL();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
    SLGLTexture* _texN;
    SLGLTexture* _texM;
    SLGLTexture* _texR;
    SLGLTexture* _texA;
    SLSkybox*    _skybox;
};
//-----------------------------------------------------------------------------

#endif
