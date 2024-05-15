#include "SLAssetLoader.h"

#include "SLGLTexture.h"
#include "SLSkybox.h"
#include "SLAssimpImporter.h"
#include "SLScene.h"

//-----------------------------------------------------------------------------
SLAssetLoader::SLAssetLoader(SLstring modelPath,
                             SLstring texturePath,
                             SLstring shaderPath)
  : _modelPath(modelPath),
    _texturePath(texturePath),
    _shaderPath(shaderPath),
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
void SLAssetLoader::addTextureToLoad(SLGLTexture*& texture,
                                     SLstring      imageFilename,
                                     SLint         min_filter,
                                     SLint         mag_filter,
                                     SLTextureType type,
                                     SLint         wrapS,
                                     SLint         wrapT)
{
    _loadTasks.push_back([this,
                          &texture,
                          imageFilename,
                          min_filter,
                          mag_filter,
                          type,
                          wrapS,
                          wrapT]
                         { texture = new SLGLTexture(_scene->assetManager(),
                                                     _texturePath + imageFilename,
                                                     min_filter,
                                                     mag_filter,
                                                     type,
                                                     wrapS,
                                                     wrapT); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addTextureToLoad(SLGLTexture*&   texture,
                                     const SLstring& filenameXPos,
                                     const SLstring& filenameXNeg,
                                     const SLstring& filenameYPos,
                                     const SLstring& filenameYNeg,
                                     const SLstring& filenameZPos,
                                     const SLstring& filenameZNeg,
                                     SLint           min_filter,
                                     SLint           mag_filter,
                                     SLTextureType   type)
{
    _loadTasks.push_back([this,
                          &texture,
                          filenameXPos,
                          filenameXNeg,
                          filenameYPos,
                          filenameYNeg,
                          filenameZPos,
                          filenameZNeg,
                          min_filter,
                          mag_filter,
                          type]
                         { texture = new SLGLTexture(_scene->assetManager(),
                                                     _texturePath + filenameXPos,
                                                     _texturePath + filenameXNeg,
                                                     _texturePath + filenameYPos,
                                                     _texturePath + filenameYNeg,
                                                     _texturePath + filenameZPos,
                                                     _texturePath + filenameZNeg,
                                                     min_filter,
                                                     mag_filter,
                                                     type); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addTextureToLoad(SLGLTexture*&   texture,
                                     SLint           depth,
                                     const SLstring& imageFilename,
                                     SLint           min_filter,
                                     SLint           mag_filter,
                                     SLint           wrapS,
                                     SLint           wrapT,
                                     const SLstring& name,
                                     SLbool          loadGrayscaleIntoAlpha)

{
    _loadTasks.push_back([this,
                          &texture,
                          depth,
                          imageFilename,
                          min_filter,
                          mag_filter,
                          wrapS,
                          wrapT,
                          name,
                          loadGrayscaleIntoAlpha]
                         { texture = new SLGLTexture(_scene->assetManager(),
                                                     depth,
                                                     _texturePath + imageFilename,
                                                     min_filter,
                                                     mag_filter,
                                                     wrapS,
                                                     wrapT,
                                                     name,
                                                     loadGrayscaleIntoAlpha); });
}
//-----------------------------------------------------------------------------
/*! Method for adding a 3D texture from a vector of images to load in parallel
 * thread.
 * @param texture Pointer to SLGLTexture to return
 * @param imageFilenames Vector of texture image files. If only filenames are
 * passed they will be searched on the SLGLTexture::defaultPath.
 * @param min_filter Minification filter constant from OpenGL
 * @param mag_filter Magnification filter constant from OpenGL
 * @param wrapS Texture wrapping in S direction (OpenGL constant)
 * @param wrapT Texture wrapping in T direction (OpenGL constant)
 * @param name Name of the 3D texture
 * @param loadGrayscaleIntoAlpha Flag if grayscale image should be loaded into
 * alpha channel.
 */
void SLAssetLoader::addTextureToLoad(SLGLTexture*&    texture,
                                     const SLVstring& imageFilenames,
                                     SLint            min_filter,
                                     SLint            mag_filter,
                                     SLint            wrapS,
                                     SLint            wrapT,
                                     const SLstring&  name,
                                     SLbool           loadGrayscaleIntoAlpha)
{
    _loadTasks.push_back([this,
                          &texture,
                          imageFilenames,
                          min_filter,
                          mag_filter,
                          wrapS,
                          wrapT,
                          name,
                          loadGrayscaleIntoAlpha]
                         { texture = new SLGLTexture(_scene->assetManager(),
                                                     imageFilenames,
                                                     min_filter,
                                                     mag_filter,
                                                     wrapS,
                                                     wrapT,
                                                     name,
                                                     loadGrayscaleIntoAlpha); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addProgramGenericToLoad(SLGLProgram*&   program,
                                            const SLstring& vertShaderFile,
                                            const SLstring& fragShaderFile)
{
    _loadTasks.push_back([this,
                          &program,
                          vertShaderFile,
                          fragShaderFile]
                         { program = new SLGLProgramGeneric(_scene->assetManager(),
                                                            _shaderPath + vertShaderFile,
                                                            _shaderPath + fragShaderFile); });
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
                             forceCookTorranceRM); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addSkyboxToLoad(SLSkybox*&      skybox,
                                    const SLstring& hdrImageWithFullPath,
                                    SLVec2i         resolution,
                                    SLstring        name)
{
    _loadTasks.push_back([this,
                          &skybox,
                          hdrImageWithFullPath,
                          resolution,
                          name]
                         { skybox = new SLSkybox(_scene->assetManager(),
                                                 _shaderPath,
                                                 hdrImageWithFullPath,
                                                 resolution,
                                                 name); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addLoadTask(SLAssetLoadTask task)
{
    _loadTasks.push_back(task);
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addSkyboxToLoad(SLSkybox*&      skybox,
                                    const SLstring& cubeMapXPos,
                                    const SLstring& cubeMapXNeg,
                                    const SLstring& cubeMapYPos,
                                    const SLstring& cubeMapYNeg,
                                    const SLstring& cubeMapZPos,
                                    const SLstring& cubeMapZNeg)
{
    _loadTasks.push_back([this,
                          &skybox,
                          cubeMapXPos,
                          cubeMapXNeg,
                          cubeMapYPos,
                          cubeMapYNeg,
                          cubeMapZPos,
                          cubeMapZNeg]
                         { skybox = new SLSkybox(_scene->assetManager(),
                                                 _shaderPath,
                                                 _texturePath + cubeMapXPos,
                                                 _texturePath + cubeMapXNeg,
                                                 _texturePath + cubeMapYPos,
                                                 _texturePath + cubeMapYNeg,
                                                 _texturePath + cubeMapZPos,
                                                 _texturePath + cubeMapZNeg); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::startLoadingAllParallel(function<void()> onDoneLoading)
{
    _onDoneLoading = onDoneLoading;
    _isDone.store(false);
    _isLoading = true;

    _worker = thread([this]
                     {
        for (const SLAssetLoadTask& task : _loadTasks)
            task();

        _isDone = true; });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::checkIfParallelLoadingIsDone()
{
    if (_isLoading && _isDone)
    {
        if (_worker)
        {
            _worker->join();
            _worker = {}; // set optional to false
        }

        _isLoading = false;
        _loadTasks.clear();
        _onDoneLoading();
    }
}
//-----------------------------------------------------------------------------