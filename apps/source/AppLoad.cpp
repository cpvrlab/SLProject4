//#############################################################################
//  File:      AppLoad.cpp
//  Date:      June 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <AppLoad.h>
#include <SLEnums.h>
#include <GlobalTimer.h>
#include <SLScene.h>
#include <SLAssetManager.h>
#include <SLAssetLoader.h>
#include <AppDemo.h>

//-----------------------------------------------------------------------------
//! These functions have to be implemented by every app or you will get a
// linking error. For the SLProject demo, they are implemented in
// AppDemoLoad.cpp
SLScene* appCreateScene(SLSceneID sceneID);
void     appBeforeSceneDelete(SLSceneView* sv, SLScene* s);
void     appBeforeSceneLoad(SLSceneView* sv, SLScene* s);
void     appBeforeSceneAssembly(SLSceneView* sv, SLScene* s);
void     appAfterSceneAssembly(SLSceneView* sv, SLScene* s);
//-----------------------------------------------------------------------------
/*!
 * Deletes the current scene and creates a new one identified by the sceneID.
 * All assets get registered for async loading while Imgui shows a progress
 * spinner in the UI. After the parallel loading the scene gets assembled back
 * in the main thread.
 * @param sv Pointer to the sceneview
 * @param sceneID Scene identifier defined in SLEnum
 */
void AppLoad::switchScene(SLSceneView* sv, SLSceneID sceneID)
{
    SLAssetManager*& am = AppDemo::assetManager;
    SLAssetLoader*&  al = AppDemo::assetLoader;

    SLfloat startLoadMS = GlobalTimer::timeMS();

    //////////////////////
    // Delete old scene //
    //////////////////////

    appBeforeSceneDelete(sv, AppDemo::scene);

    if (AppDemo::scene)
    {
        delete AppDemo::scene;
        AppDemo::scene = nullptr;
    }

    // Deactivate in general the device sensors
    AppDemo::devRot.init();
    AppDemo::devLoc.init();

    // reset existing sceneviews
    for (auto* sceneview : AppDemo::sceneViews)
        sceneview->unInit();

    // Clear all data in the asset manager
    am->clear();

    ////////////////////
    // Init new scene //
    ////////////////////

    AppDemo::sceneID = sceneID;
    SLScene* s       = appCreateScene(sceneID);

    // Initialize all preloaded stuff from SLScene
    s->init(am);

#ifndef SL_EMSCRIPTEN
    s->initOculus(AppDemo::dataPath + "shaders/");
#endif

    // Reset the global SLGLState state
    SLGLState::instance()->initAll();

    ///////////////////////////////
    // Prepare for async loading //
    ///////////////////////////////

    // Register assets on the loader that have to be loaded before assembly.
    al->scene(s);
    s->registerAssetsToLoad(*al);

    // `onDone` is a wrapper around `onDoneLoading` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneLoading, sv, s, startLoadMS);

    // Start loading assets asynchronously.
    al->loadAssetsAsync(onDone);

    appBeforeSceneLoad(sv, s);
}
//-----------------------------------------------------------------------------
void AppLoad::onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    SLAssetManager* am = AppDemo::assetManager;
    SLAssetLoader*  al = AppDemo::assetLoader;

    // Register a task to assemble the scene.
    al->addLoadTask(std::bind(&SLScene::assemble, s, am, sv));

    // `onDone` is a wrapper around `onDoneAssembling` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneAssembling, sv, s, startLoadMS);

    // Start assembling the scene asynchronously.
    al->loadAssetsAsync(onDone);

    appBeforeSceneAssembly(sv, s);
}
//-----------------------------------------------------------------------------
void AppLoad::onDoneAssembling(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    /* Assign the scene to the sceneview. The sceneview exists and is used
     * before it knows its scene. This is new since we do async loading and
     * show a spinner in the sceneview. */
    sv->scene(s);
    sv->postSceneLoad();

    // Make sure the scene view has a camera
    if (!sv->camera())
        sv->camera(sv->sceneViewCamera());

    // call onInitialize on all scene views to init the scenegraph and stats
    for (auto* sceneView : AppDemo::sceneViews)
        if (sceneView != nullptr)
            sceneView->onInitialize();

    AppDemo::scene = s;
    appAfterSceneAssembly(sv, s);

    s->loadTimeMS(GlobalTimer::timeMS() - startLoadMS);
}
//-----------------------------------------------------------------------------