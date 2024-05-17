//#############################################################################
//  File:      AppDemoSceneVideoTrackFeatures.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVIDEOFEATURE_H
#define APPDEMOSCENEVIDEOFEATURE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for feature tracking test scene
class AppDemoSceneVideoTrackFeatures : public AppScene
{
public:
    AppDemoSceneVideoTrackFeatures();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
};
//-----------------------------------------------------------------------------

#endif
