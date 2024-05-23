//#############################################################################
//  File:      AppDemoSceneRTMuttenzerBox.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENERTMUTTENZ_H
#define APPDEMOSCENERTMUTTENZ_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for the Muttenzer Box ray tracing scene
class AppDemoSceneRTMuttenzerBox : public AppScene
{
public:
    AppDemoSceneRTMuttenzerBox();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _tex1;
    SLGLProgram* _spRefl;
    SLGLProgram* _spRefr;
};
//-----------------------------------------------------------------------------

#endif
