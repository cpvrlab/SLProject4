//#############################################################################
//  File:      AppScene.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPSCENE_H
#define APPSCENE_H

#include <SLScene.h>
#include <SLAssetLoader.h>
#include <SLSceneView.h>
#include <SLAssetManager.h>

//-----------------------------------------------------------------------------
//! Abstract base class for scene loading in an application
/*!
 * The scene loading happens in 3 steps:
 * 1) Registering all expensive assets to load in registerAssetsToLoad.
 * 2) Parallel loading of assets in threads with SLAssetLoader::loadAll.
 * 3) Assembling the scene in assemble.
 * To load a scene you must therefore inherit from this class and override the
 * methods registerAssetsToLoad and assemble.
 */
class AppScene : public SLScene
{
public:
    AppScene(SLstring name) : SLScene(name) {}

    //! All assets the should be loaded in parallel must be registered in here.
    virtual void registerAssetsToLoad(SLAssetLoader& al)       = 0;

    //! After parallel loading of the assets the scene gets assembled in here.
    virtual void assemble(SLAssetManager* am, SLSceneView* sv) = 0;
};
//-----------------------------------------------------------------------------

#endif
