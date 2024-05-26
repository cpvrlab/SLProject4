//#############################################################################
//  File:      AppDemoSceneVideoTrackMediapipe.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVIDEOTRACKMEDIAPIPE_H
#define APPDEMOSCENEVIDEOTRACKMEDIAPIPE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for Mediapipe hand tracking test scene
class AppDemoSceneVideoTrackMediapipe : public SLScene
{
public:
    AppDemoSceneVideoTrackMediapipe();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
};
//-----------------------------------------------------------------------------

#endif
