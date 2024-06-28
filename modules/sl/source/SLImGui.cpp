//#############################################################################
//  File:      SLImGui.cpp
//  Purpose:   Wrapper Class around the original OpenGL3 ImGui backend
//             See also: https://github.com/ocornut/imgui
//  Date:      July 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marcus Hudritsch
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include "SLGLState.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <SLSceneView.h>
#include <SLImGui.h>
#include <SLScene.h>
#include <SLFileStorage.h>
#include <GlobalTimer.h>
#include <cstddef>
#include <backends/imgui_impl_opengl3.h>

//-----------------------------------------------------------------------------
SLfloat SLImGui::fontPropDots  = 16.0f;
SLfloat SLImGui::fontFixedDots = 13.0f;
//-----------------------------------------------------------------------------
SLImGui::SLImGui(cbOnImGuiBuild      buildCB,
                 cbOnImGuiLoadConfig loadConfigCB,
                 cbOnImGuiSaveConfig saveConfigCB,
                 int                 dpi,
                 SLIOBuffer          fontDataProp,
                 SLIOBuffer          fontDataFixed)
{
    _build      = buildCB;
    _saveConfig = saveConfigCB;
    _fontPropDots    = 13.0f;
    _fontFixedDots   = 16.0f;
    _mouseWheel      = 0.0f;
    _mousePressed[0] = false;
    _mousePressed[1] = false;
    _mousePressed[2] = false;
    _fontDataProp    = fontDataProp;
    _fontDataFixed   = fontDataFixed;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init(nullptr);

    // load config
    if (loadConfigCB)
        loadConfigCB(dpi);

    // load GUI fonts depending on the resolution
    loadFonts(SLImGui::fontPropDots, SLImGui::fontFixedDots);

    // Scale for proportional and fixed size fonts
    SLfloat dpiScaleProp  = (float)dpi / 120.0f;
    SLfloat dpiScaleFixed = (float)dpi / 142.0f;

    // Default settings for the first time
    SLImGui::fontPropDots  = std::max(16.0f * dpiScaleProp, 16.0f);
    SLImGui::fontFixedDots = std::max(13.0f * dpiScaleFixed, 13.0f);
}
//-----------------------------------------------------------------------------
SLImGui::~SLImGui()
{
    onClose();
}
//-----------------------------------------------------------------------------
//! Initializes OpenGL handles to zero and sets the ImGui key map
void SLImGui::init(const string& configPath)
{
    _fontPropDots    = 13.0f;
    _fontFixedDots   = 16.0f;
    _mouseWheel      = 0.0f;
    _mousePressed[0] = false;
    _mousePressed[1] = false;
    _mousePressed[2] = false;
    _configPath      = configPath;

    ImGuiIO& io                    = ImGui::GetIO();
    io.IniSavingRate               = 1.0f;
    io.IniFilename                 = NULL; // Disable ini config saving because we handle that ourselves
    io.KeyMap[ImGuiKey_Tab]        = K_tab;
    io.KeyMap[ImGuiKey_LeftArrow]  = K_left;
    io.KeyMap[ImGuiKey_RightArrow] = K_right;
    io.KeyMap[ImGuiKey_UpArrow]    = K_up;
    io.KeyMap[ImGuiKey_DownArrow]  = K_down;
    io.KeyMap[ImGuiKey_PageUp]     = K_pageUp;
    io.KeyMap[ImGuiKey_PageDown]   = K_pageUp;
    io.KeyMap[ImGuiKey_Home]       = K_home;
    io.KeyMap[ImGuiKey_End]        = K_end;
    io.KeyMap[ImGuiKey_Delete]     = K_delete;
    io.KeyMap[ImGuiKey_Backspace]  = K_backspace;
    io.KeyMap[ImGuiKey_Enter]      = K_enter;
    io.KeyMap[ImGuiKey_Escape]     = K_esc;
    io.KeyMap[ImGuiKey_A]          = 'A';
    io.KeyMap[ImGuiKey_C]          = 'C';
    io.KeyMap[ImGuiKey_V]          = 'V';
    io.KeyMap[ImGuiKey_X]          = 'X';
    io.KeyMap[ImGuiKey_Y]          = 'Y';
    io.KeyMap[ImGuiKey_Z]          = 'Z';

    // The screen size is set again in onResize
    io.DisplaySize             = ImVec2(0, 0);
    io.DisplayFramebufferScale = ImVec2(1, 1);

#if defined(SL_OS_ANDROID) || defined(SL_OS_MACIOS) || defined(SL_EMSCRIPTEN)
    io.MouseDrawCursor = false;
#else
    io.MouseDrawCursor = true;
#endif

    // Change default style to show the widget border
    ImGuiStyle& style     = ImGui::GetStyle();
    style.FrameBorderSize = 1;

    // Load ImGui config from imgui.ini
    SLstring iniFile = configPath + "imgui.ini";
    if (SLFileStorage::exists(iniFile, IOK_config))
    {
        SLstring iniContents = SLFileStorage::readIntoString(iniFile,
                                                             IOK_config);
        ImGui::LoadIniSettingsFromMemory(iniContents.c_str(),
                                         iniContents.size());
    }
}
//-----------------------------------------------------------------------------
//! Callback on closing the application
void SLImGui::onClose()
{
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    if (_saveConfig)
        _saveConfig();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}
//-----------------------------------------------------------------------------
//! Loads the proportional and fixed size font depending on the passed DPI
void SLImGui::loadFonts(SLfloat fontPropDotsToLoad,
                        SLfloat fontFixedDotsToLoad)
{
    _fontPropDots  = fontPropDotsToLoad;
    _fontFixedDots = fontFixedDotsToLoad;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    // Create copies of the font data because ImGUI takes ownerhip of the data.
    SLIOBuffer fontDataProp  = _fontDataProp.copy();
    SLIOBuffer fontDataFixed = _fontDataFixed.copy();

    // Load proportional font for menu and text displays
    io.Fonts->AddFontFromMemoryTTF(fontDataProp.data,
                                   static_cast<int>(fontDataProp.size),
                                   fontPropDotsToLoad);

    // Load fixed size font for statistics windows
    io.Fonts->AddFontFromMemoryTTF(fontDataFixed.data,
                                   static_cast<int>(fontDataFixed.size),
                                   fontFixedDotsToLoad);
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_CreateDeviceObjects();
}
//-----------------------------------------------------------------------------
//! Prints the compile errors in case of a GLSL compile failure
void SLImGui::printCompileErrors(SLint shaderHandle, const SLchar* src)
{
}
//-----------------------------------------------------------------------------
//! Inits a new frame for the ImGui system
void SLImGui::onInitNewFrame(SLScene* s, SLSceneView* sv)
{
    if ((SLint)SLImGui::fontPropDots != (SLint)_fontPropDots ||
        (SLint)SLImGui::fontFixedDots != (SLint)_fontFixedDots)
        loadFonts(SLImGui::fontPropDots, SLImGui::fontFixedDots);

    ImGui_ImplOpenGL3_NewFrame();

    ImGuiIO& io = ImGui::GetIO();

    // Setup time step
    SLfloat nowSec = GlobalTimer::timeS();
    io.DeltaTime   = _timeSec > 0.0 ? nowSec - _timeSec : 1.0f / 60.0f;
    if (io.DeltaTime < 0) io.DeltaTime = 1.0f / 60.0f;
    _timeSec = nowSec;

    io.MouseWheel = _mouseWheel;
    _mouseWheel   = 0.0f;

    // Start the frame
    ImGui::NewFrame();

    // Save ImGui config to imgui.ini
    if (ImGui::GetIO().WantSaveIniSettings)
    {
        SLstring    iniFile = _configPath + "imgui.ini";
        size_t      iniContentsSize;
        const char* rawIniContents = ImGui::SaveIniSettingsToMemory(&iniContentsSize);
        SLstring    iniContents(rawIniContents, rawIniContents + iniContentsSize);
        SLFileStorage::writeString(iniFile, IOK_config, iniContents);
        ImGui::GetIO().WantSaveIniSettings = false;
    }

    // Call the _build function. The whole UI is constructed here
    // This function is provided by the top-level project.
    // For the SLProject demo apps this _build function is implemented in the
    // class SLDemoGui.
    if (_build)
        _build(s, sv);
}
//-----------------------------------------------------------------------------
//! Callback if window got resized
void SLImGui::onResize(const SLRecti& viewportRect)
{
    SLGLState* stateGL         = SLGLState::instance();
    ImGuiIO&   io              = ImGui::GetIO();
    io.DisplaySize             = ImVec2((SLfloat)viewportRect.width, (SLfloat)viewportRect.height);
    io.DisplayFramebufferScale = ImVec2(1, 1);

    // Setup viewport
    if (viewportRect.isEmpty())
    {
        // (screen coordinates != framebuffer coordinates)
        int fb_width  = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
        int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        stateGL->viewport(0, 0, fb_width, fb_height);
    }
    else
    {
        GLsizei x = (GLsizei)((float)viewportRect.x * io.DisplayFramebufferScale.x);
        GLsizei y = (GLsizei)((float)viewportRect.y * io.DisplayFramebufferScale.y);
        GLsizei w = (GLsizei)((float)viewportRect.width * io.DisplayFramebufferScale.x);
        GLsizei h = (GLsizei)((float)viewportRect.height * io.DisplayFramebufferScale.y);
        stateGL->viewport(x,y,w,h);
    }
}
//-----------------------------------------------------------------------------
//! Callback for main rendering for the ImGui GUI system
void SLImGui::onPaint(const SLRecti& viewportRect)
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//-----------------------------------------------------------------------------
//! Callback on mouse button down event
void SLImGui::onMouseDown(SLMouseButton button, SLint x, SLint y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((SLfloat)x, (SLfloat)y);
    if (button == MB_left) io.MouseDown[0] = true;
    if (button == MB_right) io.MouseDown[1] = true;
    if (button == MB_middle) io.MouseDown[2] = true;
    // SL_LOG("D");
}
//-----------------------------------------------------------------------------
//! Callback on mouse button up event
void SLImGui::onMouseUp(SLMouseButton button, SLint x, SLint y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((SLfloat)x, (SLfloat)y);
    if (button == MB_left) io.MouseDown[0] = false;
    if (button == MB_right) io.MouseDown[1] = false;
    if (button == MB_middle) io.MouseDown[2] = false;
    // SL_LOG("U\n");
}
//-----------------------------------------------------------------------------
//! Updates the mouse cursor position
void SLImGui::onMouseMove(SLint xPos, SLint yPos)
{
    ImGui::GetIO().MousePos = ImVec2((SLfloat)xPos, (SLfloat)yPos);
    // SL_LOG("M");
}
//-----------------------------------------------------------------------------
//! Callback for the mouse scroll movement
void SLImGui::onMouseWheel(SLfloat yoffset)
{
    // Use fractional mouse wheel, 1.0 unit 5 lines.
    _mouseWheel += yoffset;
}
//-----------------------------------------------------------------------------
//! Callback on key press event
void SLImGui::onKeyPress(SLKey key, SLKey mod)
{
    ImGuiIO& io      = ImGui::GetIO();
    io.KeysDown[key] = true;
    io.KeyCtrl       = mod & K_ctrl ? true : false;
    io.KeyShift      = mod & K_shift ? true : false;
    io.KeyAlt        = mod & K_alt ? true : false;
}
//-----------------------------------------------------------------------------
//! Callback on key release event
void SLImGui::onKeyRelease(SLKey key, SLKey mod)
{
    ImGuiIO& io      = ImGui::GetIO();
    io.KeysDown[key] = false;
    io.KeyCtrl       = mod & K_ctrl ? true : false;
    io.KeyShift      = mod & K_shift ? true : false;
    io.KeyAlt        = mod & K_alt ? true : false;
}
//-----------------------------------------------------------------------------
//! Callback on character input
void SLImGui::onCharInput(SLuint c)
{
    ImGuiIO& io = ImGui::GetIO();
    if (c > 0 && c < 0x10000)
        io.AddInputCharacter((unsigned short)c);
}
//-----------------------------------------------------------------------------
//! Renders an extra frame with the current mouse position
void SLImGui::renderExtraFrame(SLScene*     s,
                               SLSceneView* sv,
                               SLint        mouseX,
                               SLint        mouseY)
{
    // If ImGui _build function exists render the ImGui
    if (_build)
    {
        ImGui::GetIO().MousePos = ImVec2((SLfloat)mouseX, (SLfloat)mouseY);
        onInitNewFrame(s, sv);
        onPaint(sv->viewportRect());
    }
}
//-----------------------------------------------------------------------------
