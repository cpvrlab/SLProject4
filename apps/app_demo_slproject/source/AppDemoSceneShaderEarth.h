//#############################################################################
//  File:      AppDemoSceneShaderEarth.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADEREARTH_H
#define APPDEMOSCENESHADEREARTH_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for image base lighting demo scene
class AppDemoSceneShaderEarth : public AppScene
{
public:
    AppDemoSceneShaderEarth();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLProgram* _sp;
    SLGLTexture* _texC;
    SLGLTexture* _texN;
    SLGLTexture* _texH;
    SLGLTexture* _texG;
    SLGLTexture* _texNC;
    SLGLTexture* _texClC;
};
//-----------------------------------------------------------------------------

#endif
