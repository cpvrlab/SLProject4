/**
 * \file      AppLoad.h
 * \brief     The AppLoad class is responsible for the async loading of scenes
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPLOAD_H
#define APPLOAD_H

#include <SLSceneView.h>
#include <SLEnums.h>

//----------------------------------------------------------------------------
//! The AppLoad class is responsible for the async loading of scenes
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