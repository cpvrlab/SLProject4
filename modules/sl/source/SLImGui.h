//#############################################################################
//  File:      gl/SLImGui.cpp
//  Purpose:   Wrapper Class around the external ImGui GUI-framework
//             See also: https://github.com/ocornut/imgui
//  Date:      October 2015
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLIMGUI_H
#define SLIMGUI_H

#include <SL.h>
#include <SLEnums.h>
#include <SLVec2.h>
#include <math/SLRect.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <SLUiInterface.h>
#include "SLFileStorage.h"

class SLScene;
class SLSceneView;

//! Callback function typedef for ImGui build function
typedef void(SL_STDCALL* cbOnImGuiBuild)(SLScene* s, SLSceneView* sv);

//! Callback function typedef for ImGui load config function
typedef void(SL_STDCALL* cbOnImGuiLoadConfig)(int dpi);

//! Callback function typedef for ImGui save config function
typedef void(SL_STDCALL* cbOnImGuiSaveConfig)();

//-----------------------------------------------------------------------------
//! ImGui Interface class for forwarding all events to the original ImGui Handlers
/*! ImGui is a super easy GUI library for the rendering of a UI with OpenGL.
For more information see: https://github.com/ocornut/imgui\n
\n
This class provides only the interface into ImGui. In the event handlers of
SLSceneView the according callback in ImGui is called.\n
Compared to the older SLGLImGui class this class contains no OpenGL code and
only calls the original backend functions in ImGui_impl_OpenGL3.cpp provided
by the ImGui framework.\n
There is no UI drawn with this class. It must be defined in another class
that provides the build function. For the Demo apps this is done in the file
AppDemoGui and the build function is passed e.g. in glfwMain function of the
app-Demo-SLProject project.\n
\n
The full call stack for rendering one frame is:\n
- The top-level onPaint of the app (Win, Linux, MacOS, Android or iOS)
  - slUpdateAndPaint: C-Interface function of SLProject
    - SLSceneView::onPaint: Main onPaint function of a sceneview
      - SLImGui::onInitNewFrame: Initializes a new GUI frame
        - ImGui::NewFrame()
        - SLImGui::build: The UI build function
      - ... normal scene rendering of SLProject
      - SLSceneView::draw2DGL:
        - ImGui::render
          - SLImGui::onPaint(ImGui::GetDrawData())
          - AppDemoGui::buildDemoGui: Builds the full UI
*/

class SLImGui : public SLUiInterface
{
public:
    SLImGui(cbOnImGuiBuild      buildCB,
            cbOnImGuiLoadConfig loadConfigCB,
            cbOnImGuiSaveConfig saveConfigCB,
            int                 dpi,
            SLIOBuffer          fontDataProp,
            SLIOBuffer          fontDataFixed);
    ~SLImGui() override;
    void init(const string& configPath) override;

    void onInitNewFrame(SLScene* s, SLSceneView* sv) override;
    void onResize(const SLRecti& viewport) override;
    void onPaint(const SLRecti& viewport) override;
    void onMouseDown(SLMouseButton button, SLint x, SLint y) override;
    void onMouseUp(SLMouseButton button, SLint x, SLint y) override;
    void onMouseMove(SLint xPos, SLint yPos) override;
    void onMouseWheel(SLfloat yoffset) override;
    void onKeyPress(SLKey key, SLKey mod) override;
    void onKeyRelease(SLKey key, SLKey mod) override;
    void onCharInput(SLuint c) override;
    void onClose() override;
    void renderExtraFrame(SLScene* s, SLSceneView* sv, SLint mouseX, SLint mouseY) override;
    bool doNotDispatchKeyboard() override { return ImGui::GetIO().WantCaptureKeyboard; }
    bool doNotDispatchMouse() override { return ImGui::GetIO().WantCaptureMouse; }
    void loadFonts(SLfloat fontPropDots, SLfloat fontFixedDots);
    void drawMouseCursor(bool doDraw) override { ImGui::GetIO().MouseDrawCursor = doDraw; }

    // Default font dots
    static SLfloat fontPropDots;  //!< Default font size of proportional font
    static SLfloat fontFixedDots; //!< Default font size of fixed size font

private:
    void deleteOpenGLObjects();
    void createOpenGLObjects();
    void printCompileErrors(SLint         shaderHandle,
                            const SLchar* src);

    // gui build function pattern
    cbOnImGuiBuild _build = nullptr;

    // save config callback
    cbOnImGuiSaveConfig _saveConfig = nullptr;

    SLfloat    _timeSec;         //!< Time in seconds
    SLVec2f    _mousePosPX;      //!< Mouse cursor position
    SLfloat    _mouseWheel;      //!< Mouse wheel position
    SLbool     _mousePressed[3]; //!< Mouse button press state
    SLfloat    _fontPropDots;    //!< Active font size of proportional font
    SLfloat    _fontFixedDots;   //!< Active font size of fixed size font
    SLstring   _configPath;      //!< Path to config files
    SLIOBuffer _fontDataProp;    //!< Raw data of proportional font file
    SLIOBuffer _fontDataFixed;   //!< Raw data of fixed size font file
};
//-----------------------------------------------------------------------------
#endif
