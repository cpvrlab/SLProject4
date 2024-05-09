//#############################################################################
//  File:      AppDemoSceneTextureBlend.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENETEXTUREBLEND_H
#define APPDEMOSCENETEXTUREBLEND_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
// Class for texture blending scene
class AppDemoSceneTextureBlend : public AppScene
{
public:
    AppDemoSceneTextureBlend();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _t1;
    SLGLTexture* _t2;
    SLGLProgram* _sp;
};
//-----------------------------------------------------------------------------

#endif
