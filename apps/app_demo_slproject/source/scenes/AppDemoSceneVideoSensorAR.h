//#############################################################################
//  File:      AppDemoSceneVideoSensorAR.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEVIDEOSENSORAR_H
#define APPDEMOSCENEVIDEOSENSORAR_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for devices IMU and GPS Sensors test scene.
class AppDemoSceneVideoSensorAR : public SLScene
{
public:
    AppDemoSceneVideoSensorAR();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
};
//-----------------------------------------------------------------------------

#endif
