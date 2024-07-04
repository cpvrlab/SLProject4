/**
 * \file      AppDemoSceneView.h
 * \date      August 2019
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLSceneView.h>

class SLScene;
//-----------------------------------------------------------------------------
/*!
 The SLSceneView class is inherited because we override here the default
 event-handling for onMouseDown.
*/
class AppDemoSceneView : public SLSceneView
{
public:
    AppDemoSceneView(SLScene* s, int dpi, SLInputManager& inputManager);

    // Overwritten from SLSceneView
    SLbool onKeyPress(SLKey key, SLKey mod) final;
    SLbool onKeyRelease(SLKey key, SLKey mod) final;
    SLbool onMouseDown(SLMouseButton button, SLint x, SLint y, SLKey mod) final;
    SLbool grab = false;
};
//-----------------------------------------------------------------------------
