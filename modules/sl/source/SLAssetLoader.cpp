#include "SLAssetLoader.h"

#include "SLGLTexture.h"
#include "SLAssimpImporter.h"
#include "SLScene.h"

//-----------------------------------------------------------------------------
SLAssetLoader::SLAssetLoader(SLScene* scene,
                             SLstring modelPath,
                             SLstring texturePath)
  : _scene(scene), _modelPath(modelPath), _texturePath(texturePath)
{
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
void SLAssetLoader::loadAll()
{
    for (const SLAssetLoadTask& task : _loadTasks)
        task();

    _loadTasks.clear();
}
//-----------------------------------------------------------------------------