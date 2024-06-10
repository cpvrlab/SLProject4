//#############################################################################
//  File:      App.h
//  Date:      June 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APP_H
#define APP_H

#include <SL.h>
#include <SLEnums.h>

//-----------------------------------------------------------------------------
class SLScene;
class SLSceneView;
class SLInputManager;

//-----------------------------------------------------------------------------

namespace App
{
typedef SLSceneView* (*OnNewSceneViewCallback)(SLScene*        scene,
                                               int             curDPI,
                                               SLInputManager& inputManager);
typedef bool (*OnUpdateCallback)(SLSceneView* sv);
typedef void (*OnGuiBuildCallback)(SLScene* s, SLSceneView* sv);
typedef void (*OnGuiLoadConfigCallback)(SLint dotsPerInch);
typedef void (*OnGuiSaveConfigCallback)();

struct Config
{
    int                     argc            = 0;
    char**                  argv            = nullptr;
    SLint                   windowWidth     = 1280;
    SLint                   windowHeight    = 720;
    SLstring                windowTitle     = "SLProject Application";
    SLint                   numSamples      = 4;
    SLSceneID               startSceneID    = SID_Empty;
    OnNewSceneViewCallback  onNewSceneView  = nullptr;
    OnUpdateCallback        onUpdate        = nullptr;
    OnGuiBuildCallback      onGuiBuild      = nullptr;
    OnGuiLoadConfigCallback onGuiLoadConfig = nullptr;
    OnGuiSaveConfigCallback onGuiSaveConfig = nullptr;
};

int run(Config config);
};
//-----------------------------------------------------------------------------

#endif