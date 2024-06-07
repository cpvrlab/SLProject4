//#############################################################################
//  File:      AppNodeScene.h
//  Date:      June 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPNODESCENE_H
#define APPNODESCENE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
class AppNodeScene : public SLScene
{
public:
    AppNodeScene();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _axesNode;
};
//-----------------------------------------------------------------------------

#endif
