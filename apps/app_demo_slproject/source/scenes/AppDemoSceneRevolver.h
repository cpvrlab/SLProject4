//#############################################################################
//  File:      AppDemoSceneRevolver.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEREVOLVER_H
#define APPDEMOSCENEREVOLVER_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for revolver mesh scene
class AppDemoSceneRevolver : public AppScene
{
public:
    AppDemoSceneRevolver();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _tex1C;
    SLGLTexture* _tex1N;
    SLGLTexture* _tex2;
    SLGLTexture* _tex3;
    SLGLTexture* _tex4;
    SLGLTexture* _tex5;
    SLGLProgram* _sp1;
};
//-----------------------------------------------------------------------------

#endif
