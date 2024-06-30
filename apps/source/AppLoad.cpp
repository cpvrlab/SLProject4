/**
 * \file      AppLoad.cpp
 * \brief     The AppLoad class is responsible for the async loading of scenes
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <AppLoad.h>
#include <SLEnums.h>
#include <GlobalTimer.h>
#include <SLScene.h>
#include <SLAssetManager.h>
#include <SLAssetLoader.h>
#include <AppCommon.h>
#include <App.h>

//-----------------------------------------------------------------------------
/*!
 * Deletes the current scene and creates a new one identified by the sceneID.
 * All assets get registered for async loading while Imgui shows a progress
 * spinner in the UI. After the parallel loading the scene gets assembled back
 * in the main thread.
 * \param sv Pointer to the sceneview
 * \param sceneID Scene identifier defined in SLEnum
 */
void AppLoad::switchScene(SLSceneView* sv, SLSceneID sceneID)
{
    SLAssetManager*& am = AppCommon::assetManager;
    SLAssetLoader*&  al = AppCommon::assetLoader;

    SLfloat startLoadMS = GlobalTimer::timeMS();

    //////////////////////
    // Delete old scene //
    //////////////////////

    if (App::config.onBeforeSceneDelete)
        App::config.onBeforeSceneDelete(sv, AppCommon::scene);

    if (AppCommon::scene)
    {
        delete AppCommon::scene;
        AppCommon::scene = nullptr;
    }

    // Deactivate in general the device sensors
    AppCommon::devRot.init();
    AppCommon::devLoc.init();

    // reset existing sceneviews
    for (auto* sceneView : AppCommon::sceneViews)
        sceneView->unInit();

    // Clear all data in the asset manager
    am->clear();

    ////////////////////
    // Init new scene //
    ////////////////////

    AppCommon::sceneID = sceneID;
    SLScene* s       = App::config.onNewScene(sceneID);
    SL_LOG("Scene name       : %s (SceneID: %d)", 
           s->name().c_str(), 
           AppCommon::sceneID);

    // Initialize all preloaded stuff from SLScene
    s->init(am);

#ifndef SL_EMSCRIPTEN
    s->initOculus(AppCommon::dataPath + "shaders/");
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

    if (App::config.onBeforeSceneLoad)
        App::config.onBeforeSceneLoad(sv, s);
}
//-----------------------------------------------------------------------------
void AppLoad::onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS)
{
    SLAssetManager* am = AppCommon::assetManager;
    SLAssetLoader*  al = AppCommon::assetLoader;

    // Register a task to assemble the scene.
    al->addLoadTask(std::bind(&SLScene::assemble, s, am, sv));

    // `onDone` is a wrapper around `onDoneAssembling` that will be called
    // in the main thread after loading.
    auto onDone = std::bind(onDoneAssembling, sv, s, startLoadMS);

    // Start assembling the scene asynchronously.
    al->loadAssetsAsync(onDone);

    if (App::config.onBeforeSceneAssembly)
        App::config.onBeforeSceneAssembly(sv, s);
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
    for (auto* sceneView : AppCommon::sceneViews)
        if (sceneView != nullptr)
            sceneView->onInitialize();

    AppCommon::scene = s;

    if (App::config.onAfterSceneAssembly)
        App::config.onAfterSceneAssembly(sv, s);

    s->loadTimeMS(GlobalTimer::timeMS() - startLoadMS);
}
//-----------------------------------------------------------------------------