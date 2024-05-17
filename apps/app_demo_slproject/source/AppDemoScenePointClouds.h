//#############################################################################
//  File:      AppDemoScenePointClouds.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEPOINTCLOUDS_H
#define APPDEMOSCENEPOINTCLOUDS_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for point cloud test scene
class AppDemoScenePointClouds : public AppScene
{
public:
    AppDemoScenePointClouds();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLGLProgram* _sp1;
    SLGLProgram* _sp2;
};
//-----------------------------------------------------------------------------

#endif