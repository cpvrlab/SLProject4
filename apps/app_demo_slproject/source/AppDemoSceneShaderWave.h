//#############################################################################
//  File:      AppDemoSceneShaderWave.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESHADERWAVE_H
#define APPDEMOSCENESHADERWAVE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneShaderWave : public AppScene
{
public:
    AppDemoSceneShaderWave();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLProgram* _sp;
};
//-----------------------------------------------------------------------------

#endif
