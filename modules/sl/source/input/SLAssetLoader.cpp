#include "SLAssetLoader.h"

#include <mutex>
#include <iostream>

#include "SLFileStorage.h"
#include "SLGLProgramManager.h"
#include "SLGLTexture.h"
#include "SLSkybox.h"
#include "SLAssimpImporter.h"
#include "SLScene.h"
#include "SLAssetManager.h"
#include "SLDeviceLocation.h"

//-----------------------------------------------------------------------------
using std::unique_lock;
//-----------------------------------------------------------------------------
SLAssetLoader::SLAssetLoader(SLstring modelPath,
                             SLstring texturePath,
                             SLstring shaderPath,
                             SLstring fontPath)
  : _modelPath(modelPath),
    _texturePath(texturePath),
    _shaderPath(shaderPath),
    _fontPath(fontPath),
    _state(State::IDLE)
{
    auto workerFunc = [this]() {
        while (true)
        {
            // Wait until the main thread sends a message.
            // 'Sending a message' means updating the state to a value that instructs
            // the worker thread to do something.
            unique_lock lock(_messageMutex);
            _messageCondVar.wait(lock, [this]() { return _state == State::SUBMITTED || _state == State::STOPPING; });

            // Do something depending on the current state.
            if (_state == State::SUBMITTED)
            {
                // Process the tasks defined by the main thread.
                for (SLAssetLoadTask& task : _loadTasks)
                    task();

                // Notify the main thread that the worker thread is done.
                // The main thread checks this in checkIfAsyncLoadingIsDone.
                _state = State::DONE;
            }
            else if (_state == State::STOPPING)
            {
                // Stops the worker thread by jumping...
                break;
            }
        }

        // ...here and notifying the main thread that it can now join the worker thread.
        _state = State::STOPPED;
        _messageCondVar.notify_one();
    };

    _worker = std::thread(workerFunc);
}
//-----------------------------------------------------------------------------
SLAssetLoader::~SLAssetLoader()
{
    // Instruct the worker thread to stop.
    unique_lock lock(_messageMutex);
    _state = State::STOPPING;
    lock.unlock();
    _messageCondVar.notify_one();

    // Wait until the worker thread has stopped so we can join it.
    lock.lock();
    _messageCondVar.wait(lock, [this]() { return _state == State::STOPPED; });
    _worker.join();
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addRawDataToLoad(SLIOBuffer&    buffer,
                                     SLstring       filename,
                                     SLIOStreamKind kind)
{
    _loadTasks.push_back([this, &buffer, filename, kind] {
        buffer = SLFileStorage::readIntoBuffer(filename, kind);
    });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addTextureToLoad(SLGLTexture*&   texture,
                                     const SLstring& path,
                                     SLint           min_filter,
                                     SLint           mag_filter,
                                     SLTextureType   type,
                                     SLint           wrapS,
                                     SLint           wrapT)
{
    _loadTasks.push_back([this,
                          &texture,
                          path,
                          min_filter,
                          mag_filter,
                          type,
                          wrapS,
                          wrapT] { texture = new SLGLTexture(_scene->assetManager(),
                                                             path,
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
                          type] { texture = new SLGLTexture(_scene->assetManager(),
                                                            filenameXPos,
                                                            filenameXNeg,
                                                            filenameYPos,
                                                            filenameYNeg,
                                                            filenameZPos,
                                                            filenameZNeg,
                                                            min_filter,
                                                            mag_filter,
                                                            type); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addTextureToLoad(SLGLTexture*&   texture,
                                     SLint           depth,
                                     const SLstring& imagePath,
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
                          imagePath,
                          min_filter,
                          mag_filter,
                          wrapS,
                          wrapT,
                          name,
                          loadGrayscaleIntoAlpha] { texture = new SLGLTexture(_scene->assetManager(),
                                                                              depth,
                                                                              imagePath,
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
                                     const SLVstring& imagePaths,
                                     SLint            min_filter,
                                     SLint            mag_filter,
                                     SLint            wrapS,
                                     SLint            wrapT,
                                     const SLstring&  name,
                                     SLbool           loadGrayscaleIntoAlpha)
{
    _loadTasks.push_back([this,
                          &texture,
                          imagePaths,
                          min_filter,
                          mag_filter,
                          wrapS,
                          wrapT,
                          name,
                          loadGrayscaleIntoAlpha] { texture = new SLGLTexture(_scene->assetManager(),
                                                                              imagePaths,
                                                                              min_filter,
                                                                              mag_filter,
                                                                              wrapS,
                                                                              wrapT,
                                                                              name,
                                                                              loadGrayscaleIntoAlpha); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addGeoTiffToLoad(SLDeviceLocation& devLoc,
                                     const SLstring&   imageFileWithPath)
{
    _loadTasks.push_back([this,
                          &devLoc,
                          &imageFileWithPath] { devLoc.loadGeoTiff(imageFileWithPath); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addProgramToLoad(SLGLProgram*&   program,
                                     const SLstring& vertShaderPath,
                                     const SLstring& fragShaderPath)
{
    _loadTasks.push_back([this,
                          &program,
                          vertShaderPath,
                          fragShaderPath] { program = new SLGLProgramGeneric(_scene->assetManager(),
                                                                             vertShaderPath,
                                                                             fragShaderPath); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::addNodeToLoad(SLNode*&        node,
                                  const SLstring& modelPath,
                                  SLSkybox*       skybox,
                                  SLbool          deleteTexImgAfterBuild,
                                  SLbool          loadMeshesOnly,
                                  SLMaterial*     overrideMat,
                                  float           ambientFactor,
                                  SLbool          forceCookTorranceRM,
                                  SLuint          flags)
{
    _loadTasks.push_back([this,
                          &node,
                          modelPath,
                          skybox,
                          deleteTexImgAfterBuild,
                          loadMeshesOnly,
                          overrideMat,
                          ambientFactor,
                          forceCookTorranceRM,
                          flags] {
        SLAssimpImporter importer;
        node = importer.load(_scene->animManager(),
                             _scene->assetManager(),
                             modelPath,
                             _texturePath,
                             skybox,
                             deleteTexImgAfterBuild,
                             loadMeshesOnly,
                             overrideMat,
                             ambientFactor,
                             forceCookTorranceRM,
                             nullptr,
                             flags); });
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
                          name] { skybox = new SLSkybox(_scene->assetManager(),
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
                          cubeMapZNeg] { skybox = new SLSkybox(_scene->assetManager(),
                                                               _shaderPath,
                                                               _texturePath + cubeMapXPos,
                                                               _texturePath + cubeMapXNeg,
                                                               _texturePath + cubeMapYPos,
                                                               _texturePath + cubeMapYNeg,
                                                               _texturePath + cubeMapZPos,
                                                               _texturePath + cubeMapZNeg); });
}
//-----------------------------------------------------------------------------
void SLAssetLoader::loadAssetsSync()
{
    for (const SLAssetLoadTask& task : _loadTasks)
        task();

    _loadTasks.clear();
}
//-----------------------------------------------------------------------------
void SLAssetLoader::loadAssetsAsync(function<void()> onDoneLoading)
{
    _onDoneLoading = onDoneLoading;

    // Instruct the worker thread to start processing tasks.
    unique_lock lock(_messageMutex);
    _state = State::SUBMITTED;
    lock.unlock();
    _messageCondVar.notify_one();
}
//-----------------------------------------------------------------------------
void SLAssetLoader::checkIfAsyncLoadingIsDone()
{
    if (_state == State::DONE)
    {
        _state = State::IDLE;
        _loadTasks.clear();
        _onDoneLoading();
    }
}
//-----------------------------------------------------------------------------