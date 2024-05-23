//#############################################################################
//  File:      AppDemoSceneRTSpheres.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENERTSPHERES_H
#define APPDEMOSCENERTSPHERES_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for the ray tracing scene with sphere group
class AppDemoSceneRTSpheres : public AppScene
{
public:
    AppDemoSceneRTSpheres(SLSceneID sceneID);
    void    registerAssetsToLoad(SLAssetLoader& al) override;
    void    assemble(SLAssetManager* am, SLSceneView* sv) override;
    SLNode* SphereGroupRT(SLAssetManager* am,
                          SLint           depth,
                          SLfloat         x,
                          SLfloat         y,
                          SLfloat         z,
                          SLfloat         scale,
                          SLuint          resolution,
                          SLMaterial*     matGlass,
                          SLMaterial*     matRed);
private:
    SLSceneID _sceneID;
};
//-----------------------------------------------------------------------------

#endif
