/**
 * \file      App.h
 * \brief     The App namespace declares the App::Config struct and the App::run function.
 * \details   An App::Config can be passed to the run function defined
 *            Beside this is contains the callback functions typedefs.
 *            For more info on how to create a new app with SLProject see:
 *            https://github.com/cpvrlab/SLProject4/wiki/Creating-a-New-App
 *            For more info about App framework see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APP_H
#define APP_H

#include <SL.h>
#include <SLEnums.h>

#ifndef SL_OS_ANDROID
#    define SL_MAIN_FUNCTION main
#else
#    define SL_MAIN_FUNCTION slAndroidMain
#endif

//-----------------------------------------------------------------------------
class SLScene;
class SLSceneView;
class SLInputManager;
//-----------------------------------------------------------------------------
namespace App
{
typedef SLSceneView* (*OnNewSceneViewCallback)(SLScene* scene, int curDPI, SLInputManager& inputManager);
typedef SLScene* (*OnNewSceneCallback)(SLSceneID sceneID);
typedef void (*OnBeforeSceneDeleteCallback)(SLSceneView* sv, SLScene* s);
typedef void (*OnBeforeSceneLoadCallback)(SLSceneView* sv, SLScene* s);
typedef void (*OnBeforeSceneAssemblyCallback)(SLSceneView* sv, SLScene* s);
typedef void (*OnAfterSceneAssemblyCallback)(SLSceneView* sv, SLScene* s);
typedef bool (*OnUpdateCallback)(SLSceneView* sv);
typedef void (*OnGuiBuildCallback)(SLScene* s, SLSceneView* sv);
typedef void (*OnGuiLoadConfigCallback)(SLint dotsPerInch);
typedef void (*OnGuiSaveConfigCallback)();

struct Config
{
    int                           argc                  = 0;
    char**                        argv                  = nullptr;
    SLint                         windowWidth           = 1280;
    SLint                         windowHeight          = 720;
    SLstring                      windowTitle           = "SLProject Application";
    SLint                         numSamples            = 4;
    SLSceneID                     startSceneID          = SID_Empty;
    OnNewSceneViewCallback        onNewSceneView        = nullptr;
    OnNewSceneCallback            onNewScene            = nullptr;
    OnBeforeSceneDeleteCallback   onBeforeSceneDelete   = nullptr;
    OnBeforeSceneLoadCallback     onBeforeSceneLoad     = nullptr;
    OnBeforeSceneAssemblyCallback onBeforeSceneAssembly = nullptr;
    OnAfterSceneAssemblyCallback  onAfterSceneAssembly  = nullptr;
    OnUpdateCallback              onUpdate              = nullptr;
    OnGuiBuildCallback            onGuiBuild            = nullptr;
    OnGuiLoadConfigCallback       onGuiLoadConfig       = nullptr;
    OnGuiSaveConfigCallback       onGuiSaveConfig       = nullptr;
};

int run(Config config);

extern Config config;
};
//-----------------------------------------------------------------------------

#endif