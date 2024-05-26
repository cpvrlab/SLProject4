//#############################################################################
//  File:      AppDemoSceneShaderBlinn.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERBLINN_H
#define APPDEMOSCENESHADERBLINN_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for demo scene with Blinn-Phong lighting per vertex or per pixel
class AppDemoSceneShaderBlinn : public SLScene
{
public:
    AppDemoSceneShaderBlinn(SLstring name,
                            bool     perVertex);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    bool         _perVertex;
    SLGLProgram* _perVrtTm = nullptr;
    SLGLProgram* _perVrt = nullptr;
    SLGLProgram* _perPix = nullptr;
    SLGLTexture* _texC = nullptr;
    SLGLTexture* _texN = nullptr;
    SLGLTexture* _texH = nullptr;
};
//-----------------------------------------------------------------------------

#endif
