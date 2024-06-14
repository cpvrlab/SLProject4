//#############################################################################
//  File:      AppAndroid.cpp
//  Date:      June 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <App.h>
#include <SLGLState.h>
#include <SLEnums.h>
#include <SLInterface.h>
#include <AppDemo.h>
#include <SLAssetManager.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <CVCapture.h>
#include <Profiler.h>
#include <SLAssetLoader.h>

//-----------------------------------------------------------------------------
// Global variables
App::Config App::config; //!< The configuration set in App::run

//-----------------------------------------------------------------------------
int App::run(Config configuration)
{
    App::config = configuration;
    return 0;
}
//-----------------------------------------------------------------------------
