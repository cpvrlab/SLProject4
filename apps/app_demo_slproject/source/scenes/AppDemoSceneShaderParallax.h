//#############################################################################
//  File:      AppDemoSceneShaderParallax.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERPARALLAX_H
#define APPDEMOSCENESHADERPARALLAX_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for parallax bump mapping demo scene
class AppDemoSceneShaderParallax : public AppScene
{
public:
    AppDemoSceneShaderParallax();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
    SLGLTexture* _texN;
    SLGLTexture* _texH;
    SLGLProgram* _sp;
};
//-----------------------------------------------------------------------------

#endif
