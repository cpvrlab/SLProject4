/**
 * \file      gl/SLGLImGui.cpp
 * \brief   Wrapper Class around the external ImGui GUI-framework
//             See also: https://github.com/ocornut/imgui
 * \date      October 2015
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLGLIMGUI_H
#define SLGLIMGUI_H

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
//! ImGui Interface class for forwarding all events to the ImGui Handlers
/*! ImGui is a super easy GUI library for the rendering of a UI with OpenGL.
For more information see: https://github.com/ocornut/imgui\n
\n
This class provides only the interface into ImGui. In the event handlers of
SLSceneView the according callback in ImGui is called.\n
There is no UI drawn with this class. It must be defined in another class
that provides the build function. For the Demo apps this is done in the class
SLDemoGui and the build function is passed e.g. in glfwMain function of the
app-demo project.\n
\n
The full call stack for rendering one frame is:\n
- The top-level onPaint of the app (Win, Linux, MacOS, Android or iOS)
  - slUpdateAndPaint: C-Interface function of SLProject
    - SLSceneView::onPaint: Main onPaint function of a sceneview
      - SLGLImGui::onInitNewFrame: Initializes a new GUI frame
        - ImGui::NewFrame()
        - SLGLImGui::build: The UI build function
      - ... normal scene rendering of SLProject
      - SLSceneView::draw2DGL:
        - ImGui::Render
          - SLGLImGui::onPaint(ImGui::GetDrawData())
          - SLDemoGui::buildDemoGui: Builds the full UI
*/

class SLGLImGui : public SLUiInterface
{
public:
    SLGLImGui(cbOnImGuiBuild      buildCB,
              cbOnImGuiLoadConfig loadConfigCB,
              cbOnImGuiSaveConfig saveConfigCB,
              int                 dpi,
              SLIOBuffer          fontDataProp,
              SLIOBuffer          fontDataFixed);
    ~SLGLImGui() override;
    void init(const string& configPath) override;

    void onInitNewFrame(SLScene* s, SLSceneView* sv) override;
    void onResize(const SLRecti& viewportRect) override;
    void onPaint(const SLRecti& viewport) override;
    void onMouseDown(SLMouseButton button, SLint x, SLint y) override;
    void onMouseUp(SLMouseButton button, SLint x, SLint y) override;
    // returns true if it wants to capture mouse
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

    SLfloat    _timeSec;           //!< Time in seconds
    SLVec2f    _mousePosPX;        //!< Mouse cursor position
    SLfloat    _mouseWheel;        //!< Mouse wheel position
    SLbool     _mousePressed[3];   //!< Mouse button press state
    SLuint     _fontTexture;       //!< OpenGL texture id for font
    SLint      _progHandle;        //!< OpenGL handle for shader program
    SLint      _vertHandle;        //!< OpenGL handle for vertex shader
    SLint      _fragHandle;        //!< OpenGL handle for fragment shader
    SLint      _attribLocTex;      //!< OpenGL attribute location for texture
    SLint      _attribLocProjMtx;  //!< OpenGL attribute location for ???
    SLint      _attribLocPosition; //!< OpenGL attribute location for vertex pos.
    SLint      _attribLocUV;       //!< OpenGL attribute location for texture coords
    SLint      _attribLocColor;    //!< OpenGL attribute location for color
    SLuint     _vboHandle;         //!< OpenGL handle for vertex buffer object
    SLuint     _vaoHandle;         //!< OpenGL vertex array object handle
    SLuint     _elementsHandle;    //!< OpenGL handle for vertex indexes
    SLfloat    _fontPropDots;      //!< Active font size of proportional font
    SLfloat    _fontFixedDots;     //!< Active font size of fixed size font
    SLstring   _configPath;        //!< Path to config files
    SLIOBuffer _fontDataProp;      //!< Raw data of proportional font file
    SLIOBuffer _fontDataFixed;     //!< Raw data of fixed size font file
};
//-----------------------------------------------------------------------------
#endif
