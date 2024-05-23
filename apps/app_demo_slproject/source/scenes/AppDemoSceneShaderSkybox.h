//#############################################################################
//  File:      AppDemoSceneShaderSkybox.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERSKYBOX_H
#define APPDEMOSCENESHADERSKYBOX_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for image base lighting demo scene
class AppDemoSceneShaderSkybox : public AppScene
{
public:
    AppDemoSceneShaderSkybox();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLSkybox*    _skybox;
    SLGLProgram* _spRefl;
    SLGLProgram* _spRefr;
    SLNode*      _teapot;
    SLNode*      _suzanne;
};
//-----------------------------------------------------------------------------

#endif
