//#############################################################################
//  File:      AppDemoSceneRTDoF.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENERTDOF_H
#define APPDEMOSCENERTDOF_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for the depth of field ray tracing scene
class AppDemoSceneRTDoF : public AppScene
{
public:
    AppDemoSceneRTDoF();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLTexture* _tex1;
};
//-----------------------------------------------------------------------------

#endif
