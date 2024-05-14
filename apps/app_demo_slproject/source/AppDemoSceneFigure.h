//#############################################################################
//  File:      AppDemoSceneFigure.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEFIGURE_H
#define APPDEMOSCENEFIGURE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for the figure scene
class AppDemoSceneFigure : public AppScene
{
public:
    AppDemoSceneFigure();
    void           registerAssetsToLoad(SLAssetLoader& al) override;
    void           assemble(SLAssetManager* am, SLSceneView* sv) override;
    static SLNode* BuildFigureGroup(SLAssetManager* am,
                                    SLScene*        s,
                                    SLMaterial*     mat,
                                    SLbool          withAnimation);
};
//-----------------------------------------------------------------------------

#endif
