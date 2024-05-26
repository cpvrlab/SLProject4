//#############################################################################
//  File:      AppDemoSceneShaderBump.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERBUMP_H
#define APPDEMOSCENESHADERBUMP_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for normal map bump mapping
class AppDemoSceneShaderBump : public SLScene
{
public:
    AppDemoSceneShaderBump();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _texC;
    SLGLTexture* _texN;
};
//-----------------------------------------------------------------------------

#endif
