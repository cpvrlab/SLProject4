/**
 * \file      AppDemoSceneView.cpp
 * \date      August 2019
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 * \authors   Marcus Hudritsch, Michael Göttlicher
 * \copyright http://opensource.org/licenses/GPL-3.0
 */

#include "SLScript.h"

#include <AppDemoSceneView.h>
#include <AppCommon.h>
#include <AppDemoGui.h>
#include <AppDemoSceneID.h>

//-----------------------------------------------------------------------------
extern SLScript script;
//-----------------------------------------------------------------------------
AppDemoSceneView::AppDemoSceneView(SLScene*        s,
                                   int             dpi,
                                   SLInputManager& inputManager)
  : SLSceneView(s, dpi, inputManager)
{
}
//-----------------------------------------------------------------------------
/*! This method overrides the same method from the base class SLSceneView.
  It runs some app-specific code if a certain key is pressed and calls the
  default implementation from SLSceneView if it doesn't consume the event.
*/
SLbool AppDemoSceneView::onKeyPress(SLKey key, SLKey mod)
{
    if (script.onKeyPress(key))
        return true;

    // Keyboard shortcuts for next or previous sceneID loading
    if (mod & K_alt && mod & K_shift)
    {
        SLSceneView* sv = AppCommon::sceneViews[0];
        if (key == '0' && sv)
        {
            AppCommon::sceneToLoad = SID_Empty;
            return true;
        }
        else if (key == K_left && sv && AppCommon::sceneID > 0)
        {
            AppCommon::sceneToLoad = static_cast<SLSceneID>(AppCommon::sceneID - 1);
            return true;
        }
        else if (key == K_right && sv && AppCommon::sceneID < SID_MaxNoBenchmarks - 1)
        {
            AppCommon::sceneToLoad = static_cast<SLSceneID>(AppCommon::sceneID + 1);
            return true;
        }
    }

    return SLSceneView::onKeyPress(key, mod);
}
//-----------------------------------------------------------------------------
/*! This method overrides the same method from the base class SLSceneView.
  It runs some app-specific code if a certain key is pressed and calls the
  default implementation from SLSceneView if it doesn't consume the event.
*/
SLbool AppDemoSceneView::onKeyRelease(SLKey key, SLKey mod)
{
    if (script.onKeyRelease(key))
        return true;

    if (AppDemoGui::hideUI)
    {
        AppDemoGui::hideUI = false;
        return true;
    }

    return SLSceneView::onKeyRelease(key, mod);
}
//-----------------------------------------------------------------------------
/*! This method overrides the same method from the base class SLSceneView.
 Most events such as all mouse and keyboard events from the OS are forwarded to
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
    if (AppCommon::sceneID == SID_VideoCalibrateMain ||
        AppCommon::sceneID == SID_VideoCalibrateScnd)
    {
        grab = true;
    }

    return baseClassResult;
}
//-----------------------------------------------------------------------------
