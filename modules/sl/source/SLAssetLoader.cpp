#include "SLAssetLoader.h"

#include "SLGLTexture.h"
#include "SLAssimpImporter.h"
#include "SLScene.h"

//-----------------------------------------------------------------------------
SLAssetLoader::SLAssetLoader(SLstring modelPath,
                             SLstring texturePath)
  : _modelPath(modelPath),
    _texturePath(texturePath),
    _isLoading(false)
{
}
//-----------------------------------------------------------------------------
SLAssetLoader::~SLAssetLoader()
{
    if (_worker)
        _worker->join();
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addTextureToLoad(SLGLTexture*& texture, SLstring path)
{
    // clang-format off
    _loadTasks.push_back([this, &texture, path]
    {
        texture = new SLGLTexture(_scene->assetManager(), _texturePath + path);
    });
    // clang-format on
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addNodeToLoad(SLNode*&    node,
                                  SLstring    path,
                                  SLSkybox*   skybox,
                                  SLbool      deleteTexImgAfterBuild,
                                  SLbool      loadMeshesOnly,
                                  SLMaterial* overrideMat,
                                  float       ambientFactor,
                                  SLbool      forceCookTorranceRM)
{
    // clang-format off
    _loadTasks.push_back([this,
                          &node,
                          path,
                          skybox,
                          deleteTexImgAfterBuild,
                          loadMeshesOnly,
                          overrideMat,
                          ambientFactor,
                          forceCookTorranceRM]
    {
        SLAssimpImporter importer;
        node = importer.load(_scene->animManager(),
                             _scene->assetManager(),
                             _modelPath + path,
                             _texturePath,
                             skybox,
                             deleteTexImgAfterBuild,
                             loadMeshesOnly,
                             overrideMat,
                             ambientFactor,
                             forceCookTorranceRM);
    });
    // clang-format on
}
//-----------------------------------------------------------------------------
void SLAssetLoader::loadAll(std::function<void()> onDoneLoading)
{
    _onDoneLoading = onDoneLoading;
    _isDone.store(false);
    _isLoading = true;

    _worker = std::thread([this] {
        for (const SLAssetLoadTask& task : _loadTasks)
            task();

        _isDone.store(true);
    });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::update()
{
    if (_isLoading && _isDone.load())
    {
        _worker->join();
        _worker = {};

        _isLoading = false;
        _loadTasks.clear();
        _onDoneLoading();
    }
}
//-----------------------------------------------------------------------------