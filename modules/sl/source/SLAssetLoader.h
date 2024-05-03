#ifndef SLASSETLOADER_H
#define SLASSETLOADER_H

#include <condition_variable>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <optional>

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
    SLAssetLoader(SLstring modelPath, SLstring texturePath);
    ~SLAssetLoader();

    bool isLoading() const { return _isLoading; }

    void scene(SLScene* scene) { _scene = scene; }

    void addTextureToLoad(SLGLTexture*& texture, SLstring path);
    void addNodeToLoad(SLNode*&    node,
                       SLstring    path,
                       SLSkybox*   skybox                 = nullptr,
                       SLbool      deleteTexImgAfterBuild = false,
                       SLbool      loadMeshesOnly         = true,
                       SLMaterial* overrideMat            = nullptr,
                       float       ambientFactor          = 0.0f,
                       SLbool      forceCookTorranceRM    = false);
    void loadAll(std::function<void()> onDone);
    void update();

private:
    SLScene*        _scene;
    SLAssetManager* _am;
    SLstring        _modelPath;
    SLstring        _texturePath;

    SLVAssetLoadTask _loadTasks;
    bool             _isLoading;

    std::atomic_bool           _isDone;
    std::optional<std::thread> _worker;
    std::function<void()>      _onDoneLoading;
};
//-----------------------------------------------------------------------------

#endif