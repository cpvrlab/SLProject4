/**
 * \file      AppDemoSceneView.cpp
 * \date      August 2019
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Michael Göttlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
*/

#include <AppDemoSceneView.h>
#include <AppDemo.h>
#include <AppDemoGui.h>

//-----------------------------------------------------------------------------
AppDemoSceneView::AppDemoSceneView(SLScene*        s,
                                   int             dpi,
                                   SLInputManager& inputManager)
  : SLSceneView(s, dpi, inputManager)
{
}
//-----------------------------------------------------------------------------
SLbool AppDemoSceneView::onKeyPress(SLKey key, SLKey mod)
{
    // Keyboard shortcuts for next or previous sceneID loading
    if (mod & K_alt && mod & K_shift)
    {
        SLSceneView* sv = AppDemo::sceneViews[0];
        if (key == '0' && sv)
        {
            AppDemo::sceneToLoad = SID_Empty;
            return true;
        }
        else if (key == K_left && sv && AppDemo::sceneID > 0)
        {
            AppDemo::sceneToLoad = static_cast<SLSceneID>(AppDemo::sceneID - 1);
            return true;
        }
        else if (key == K_right && sv && AppDemo::sceneID < SID_MaxNoBenchmarks - 1)
        {
            AppDemo::sceneToLoad = static_cast<SLSceneID>(AppDemo::sceneID + 1);
            return true;
        }
    }

    return SLSceneView::onKeyPress(key, mod);
}
//-----------------------------------------------------------------------------
SLbool AppDemoSceneView::onKeyRelease(SLKey key, SLKey mod)
{
    if (AppDemoGui::hideUI)
    {
        AppDemoGui::hideUI = false;
        return true;
    }

    return SLSceneView::onKeyRelease(key, mod);
}
//-----------------------------------------------------------------------------
/*! This method overrides the same method from the base class SLSceneView.
 Most events such as all mouse and keyboard events from the OS is forwarded to
 SLSceneview. SLSceneview implements a default behaviour. If you want a
 different or additional behaviour for a certain eventhandler you have to sub-
 class SLSceneView and override the eventhandler.
 Because all video processing (capturing and calibration) is handled outside
 of the core SLProject we need to add an additional handling for mouse down
 withing the calibration routine.
 */
SLbool AppDemoSceneView::onMouseDown(SLMouseButton button,
                                     SLint         x,
                                     SLint         y,
                                     SLKey         mod)
{
    // Call base class event-handler for default mouse and touchdown behaviour
    bool baseClassResult = SLSceneView::onMouseDown(button, x, y, mod);

    // Grab image during calibration if calibration stream is running
    if (AppDemo::sceneID == SID_VideoCalibrateMain ||
        AppDemo::sceneID == SID_VideoCalibrateScnd)
    {
        grab = true;
    }

    return baseClassResult;
}
//-----------------------------------------------------------------------------
