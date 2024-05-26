//#############################################################################
//  File:      AppDemoSceneLotsOfNodes.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENELOTSOFNODES_H
#define APPDEMOSCENELOTSOFNODES_H

#include <SLScene.h>
#include <SLGLTexture.h>

//-----------------------------------------------------------------------------
//! Class for scene with lots of nodes
class AppDemoSceneLotsOfNodes : public SLScene
{
public:
    AppDemoSceneLotsOfNodes();
    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    static const int _NUM_MAT = 20;
    SLGLTexture*     _texC[_NUM_MAT];
};
//-----------------------------------------------------------------------------

#endif
