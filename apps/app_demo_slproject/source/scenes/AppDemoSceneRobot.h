//#############################################################################
//  File:      AppDemoSceneRobot.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEROBOT_H
#define APPDEMOSCENEROBOT_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for a robot arm test scene
class AppDemoSceneRobot : public AppScene
{
public:
    AppDemoSceneRobot();

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*   _robot;
};
//-----------------------------------------------------------------------------

#endif
