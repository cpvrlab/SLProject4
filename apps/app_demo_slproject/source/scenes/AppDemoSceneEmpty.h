//#############################################################################
//  File:      AppDemoSceneEmpty.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEEMPTY_H
#define APPDEMOSCENEEMPTY_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
class AppDemoSceneEmpty : public SLScene
{
public:
    AppDemoSceneEmpty();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;
};
//-----------------------------------------------------------------------------

#endif
