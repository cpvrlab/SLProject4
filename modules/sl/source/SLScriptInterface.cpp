#include "SLScriptInterface.h"

#include "SLScene.h"

void* slScriptContextUserData(SLScriptContext* context)
{
    return context->userData;
}
//-----------------------------------------------------------------------------
void slScriptContextSetUserData(SLScriptContext* context, void* userData)
{
    context->userData = userData;
}
//-----------------------------------------------------------------------------
SLScene* slScriptContextScene(SLScriptContext* context)
{
    return context->scene;
}
//-----------------------------------------------------------------------------
int slScriptContextSceneID(SLScriptContext* context)
{
    return context->sceneID;
}
//-----------------------------------------------------------------------------
const char* slScriptSceneName(SLScene* scene)
{
    return scene->name().c_str();
}
//-----------------------------------------------------------------------------
SLNode* slScriptSceneFindNodeByName(SLScene* scene, const char* name)
{
    return scene->root3D()->findChild<SLNode>(name, true);
}
//-----------------------------------------------------------------------------
SLScriptVec3 slScriptNodePosition(SLNode* node)
{
    SLScriptVec3 result;
    result.x = node->translationWS().x;
    result.y = node->translationWS().y;
    result.z = node->translationWS().z;
    return result;
}
//-----------------------------------------------------------------------------
void slScriptNodeSetPosition(SLNode* node, SLScriptVec3 position)
{
    node->translation(SLVec3f(position.x, position.y, position.z), TS_world);
}
//-----------------------------------------------------------------------------
void slScriptNodeLookAt(SLNode* node, SLScriptVec3 target, SLScriptVec3 up)
{
    node->lookAt(SLVec3f(target.x, target.y, target.z),
                 SLVec3f(up.x, up.y, up.z),
                 TS_world);
}
//-----------------------------------------------------------------------------
