#ifndef SLASSETLOADER_H
#define SLASSETLOADER_H

#include <vector>
#include <functional>

#include <SL.h>

class SLScene;
class SLAssetManager;
class SLGLTexture;
class SLNode;
class SLSkybox;
class SLMaterial;

//-----------------------------------------------------------------------------
typedef std::function<void()>        SLAssetLoadTask;
typedef std::vector<SLAssetLoadTask> SLVAssetLoadTask;
//-----------------------------------------------------------------------------
class SLAssetLoader
{
public:
    SLAssetLoader(SLScene* scene, SLstring modelPath, SLstring texturePath);

    void addTextureToLoad(SLGLTexture*& texture, SLstring path);

    void addNodeToLoad(SLNode*&    node,
                       SLstring    path,
                       SLSkybox*   skybox                 = nullptr,
                       SLbool      deleteTexImgAfterBuild = false,
                       SLbool      loadMeshesOnly         = true,
                       SLMaterial* overrideMat            = nullptr,
                       float       ambientFactor          = 0.0f,
                       SLbool      forceCookTorranceRM    = false);

    void loadAll();

private:
    SLVAssetLoadTask _loadTasks;

    SLScene*        _scene;
    SLAssetManager* _am;
    SLstring        _modelPath;
    SLstring        _texturePath;
};
//-----------------------------------------------------------------------------

#endif