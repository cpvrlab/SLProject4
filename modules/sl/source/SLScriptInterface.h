#ifndef SLSCRIPTINTERFACE_H
#define SLSCRIPTINTERFACE_H

#include "SLEnums.h"

//-----------------------------------------------------------------------------
class SLScene;
class SLNode;
//-----------------------------------------------------------------------------
struct SLScriptContext
{
    void*     userData;
    SLScene*  scene;
    SLSceneID sceneID;
};
//-----------------------------------------------------------------------------
struct SLScriptVec3
{
    float x;
    float y;
    float z;
};
//-----------------------------------------------------------------------------
extern "C" {
void*         slScriptContextUserData(SLScriptContext* context);
void          slScriptContextSetUserData(SLScriptContext* context, void* userData);
SLScene*      slScriptContextScene(SLScriptContext* context);
int           slScriptContextSceneID(SLScriptContext* context);
const char*   slScriptSceneName(SLScene* scene);
SLNode*       slScriptSceneFindNodeByName(SLScene* scene, const char* name);
SLScriptVec3 slScriptNodePosition(SLNode* node);
void          slScriptNodeSetPosition(SLNode* node, SLScriptVec3 position);
void          slScriptNodeLookAt(SLNode* node, SLScriptVec3 target, SLScriptVec3 up);
}
//-----------------------------------------------------------------------------

#endif