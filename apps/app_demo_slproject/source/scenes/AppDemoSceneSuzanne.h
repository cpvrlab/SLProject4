//#############################################################################
//  File:      AppDemoSceneSuzanne.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPDEMOSCENESUZANNE_H
#define APPDEMOSCENESUZANNE_H

#include <AppScene.h>

//-----------------------------------------------------------------------------
//! Class for all variants of generated shaders on the Suzanne head
class AppDemoSceneSuzanne : public AppScene
{
public:
    AppDemoSceneSuzanne(SLstring name,
                        bool     textureMapping,
                        bool     normalMapping,
                        bool     occlusionMapping,
                        bool     shadowMapping);

    void registerAssetsToLoad(SLAssetLoader& al) override;
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode* _suzanneInCube;
    bool    _textureMapping;
    bool    _normalMapping;
    bool    _occlusionMapping;
    bool    _shadowMapping;
};
//-----------------------------------------------------------------------------

#endif
