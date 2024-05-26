//#############################################################################
//  File:      AppDemoSceneAnimNodeMass2.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENEANIMMASS2_H
#define APPDEMOSCENEANIMMASS2_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for benchmark mass animation test scene
class AppDemoSceneAnimNodeMass2 : public SLScene
{
public:
    AppDemoSceneAnimNodeMass2();
    void    registerAssetsToLoad(SLAssetLoader& al) override;
    void    assemble(SLAssetManager* am, SLSceneView* sv) override;
    SLNode* RotatingSpheres(SLAssetManager* am,
                                SLScene*        s,
                                SLint           depth,
                                SLfloat         x,
                                SLfloat         y,
                                SLfloat         z,
                                SLfloat         scale,
                                SLuint          resolution,
                                SLVMaterial&    mat);
};
//-----------------------------------------------------------------------------

#endif
