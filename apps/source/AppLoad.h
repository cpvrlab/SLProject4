//#############################################################################
//  File:      AppLoad.h
//  Date:      June 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef APPLOAD_H
#define APPLOAD_H

#include <SLSceneView.h>
#include <SLEnums.h>

//----------------------------------------------------------------------------
class AppLoad
{
public:
    static void switchScene(SLSceneView* sv, SLSceneID sceneID);

private:
    static void onDoneLoading(SLSceneView* sv, SLScene* s, SLfloat startLoadMS);
    static void onDoneAssembling(SLSceneView* sv, SLScene* s, SLfloat startLoadMS);
};
//-----------------------------------------------------------------------------

#endif