/**
 * \file      AppNodeGui.cpp
 * \date      Summer 2017
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include "AppNodeGui.h"
#include "AppNodeSceneView.h"
#include <SLScene.h>
#include <SLSceneView.h>
#include <imgui.h>
//-----------------------------------------------------------------------------
SLstring AppNodeGui::infoText = "";
//-----------------------------------------------------------------------------
//! Creates the ImGui UI.
/*! This function must be passed as void* pointer to the slCreateSceneView
function. It is called in SLSceneView::onPaint for each frame.
*/
void AppNodeGui::build(SLScene* s, SLSceneView* sv)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene Information",
                 0,
                 ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_AlwaysAutoResize |
                   ImGuiWindowFlags_NoMove);
    ImGui::TextUnformatted(infoText.c_str());
    ImGui::End();
}
//-----------------------------------------------------------------------------
