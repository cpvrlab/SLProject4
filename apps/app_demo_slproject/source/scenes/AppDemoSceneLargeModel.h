//#############################################################################
//  File:      AppDemoSceneLargeModel.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENELARGEMODEL_H
#define APPDEMOSCENELARGEMODEL_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for large model scene
class AppDemoSceneLargeModel : public AppScene
{
public:
    AppDemoSceneLargeModel();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _dragonModel;
};
//-----------------------------------------------------------------------------

#endif
