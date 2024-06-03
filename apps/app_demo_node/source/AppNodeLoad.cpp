// #############################################################################
//   File:      AppNodeLoad.cpp
//   Date:      February 2018
//   Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//   Authors:   Marcus Hudritsch
//   License:   This software is provided under the GNU General Public License
//              Please visit: http://opensource.org/licenses/GPL-3.0
// #############################################################################

#include <SLEnums.h>
#include <SLScene.h>
#include <SLSceneView.h>
#include <AppNodeScene.h>

//-----------------------------------------------------------------------------
SLScene* appCreateScene(SLSceneID sceneID)
{
    return new AppNodeScene();
}
//-----------------------------------------------------------------------------
void appBeforeSceneDelete(SLSceneView* sv, SLScene* s)
{
}
//-----------------------------------------------------------------------------
void appBeforeSceneLoad(SLSceneView* sv, SLScene* s)
{
}
//-----------------------------------------------------------------------------
void appBeforeSceneAssembly(SLSceneView* sv, SLScene* s)
{
}
//-----------------------------------------------------------------------------
void appAfterSceneAssembly(SLSceneView* sv, SLScene* s)
{
}
//-----------------------------------------------------------------------------