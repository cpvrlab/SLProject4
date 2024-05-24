//#############################################################################
//  File:      SLAssetLoader.h
//  Date:      May 2024
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/SLProject-Coding-Style
//  Authors:   Marino von Wattenwyl
//  License:   This software is provided under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLASSETLOADER_H
#define SLASSETLOADER_H

#include "SLFileStorage.h"

#include <condition_variable>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <optional>

#include "SL.h"
#include "SLGLTexture.h"
#include "SLFileStorage.h"

class SLScene;
class SLAssetManager;
class SLNode;
class SLSkybox;
class SLMaterial;
class SLDeviceLocation;

using std::atomic;
using std::condition_variable;
using std::function;
using std::mutex;
using std::optional;
using std::thread;

//-----------------------------------------------------------------------------
typedef function<void()>        SLAssetLoadTask;
typedef vector<SLAssetLoadTask> SLVAssetLoadTask;
//-----------------------------------------------------------------------------
class SLAssetLoader
{
private:
    enum class State
    {
        IDLE,
        SUBMITTED,
        WORKING,
        DONE,
        STOPPING,
        STOPPED
    };

public:
    SLAssetLoader(SLstring modelPath,
                  SLstring texturePath,
                  SLstring shaderPath,
                  SLstring fontPath);
    ~SLAssetLoader();

    // Setters
    void scene(SLScene* scene) { _scene = scene; }

    // Getters
    bool     isLoading() const { return _state != State::IDLE; }
    SLstring modelPath() const { return _modelPath; }
    SLstring shaderPath() const { return _shaderPath; }
    SLstring texturePath() const { return _texturePath; }

    void addRawDataToLoad(SLIOBuffer&    buffer,
                          SLstring       filename,
                          SLIOStreamKind kind);

    //! Add 2D textures with internal image allocation
    void addTextureToLoad(SLGLTexture*&   texture,
                          const SLstring& path,
                          const SLstring& imageFilename,
                          SLint           min_filter = GL_LINEAR_MIPMAP_LINEAR,
                          SLint           mag_filter = GL_LINEAR,
                          SLTextureType   type       = TT_unknown,
                          SLint           wrapS      = GL_REPEAT,
                          SLint           wrapT      = GL_REPEAT);

    //! Add cube map texture with internal image allocation
    void addTextureToLoad(SLGLTexture*&   texture,
                          const SLstring& path,
                          const SLstring& imageFilenameXPos,
                          const SLstring& imageFilenameXNeg,
                          const SLstring& imageFilenameYPos,
                          const SLstring& imageFilenameYNeg,
                          const SLstring& imageFilenameZPos,
                          const SLstring& imageFilenameZNeg,
                          SLint           min_filter = GL_LINEAR,
                          SLint           mag_filter = GL_LINEAR,
                          SLTextureType   type       = TT_unknown);

    //! Add 3D texture from a single file with depth as 3rd dimension
    void addTextureToLoad(SLGLTexture*&   texture,
                          SLint           depth,
                          const SLstring& path,
                          const SLstring& imageFilename,
                          SLint           min_filter             = GL_LINEAR,
                          SLint           mag_filter             = GL_LINEAR,
                          SLint           wrapS                  = GL_REPEAT,
                          SLint           wrapT                  = GL_REPEAT,
                          const SLstring& name                   = "3D-Texture",
                          SLbool          loadGrayscaleIntoAlpha = false);

    //! Add 3D texture from a vector of files
    void addTextureToLoad(SLGLTexture*&    texture,
                          const SLVstring& imageFilenames,
                          SLint            min_filter,
                          SLint            mag_filter,
                          SLint            wrapS,
                          SLint            wrapT,
                          const SLstring&  name,
                          SLbool           loadGrayscaleIntoAlpha);

    //! Add GeoTiff file to load for the SLDevLocation
    void addGeoTiffToLoad(SLDeviceLocation& devLoc,
                          const SLstring& imageFileWithPath);

    //! Add mesh from file to load via assimp loader
    void addNodeToLoad(SLNode*&        node,
                       const SLstring& modelPath,
                       const SLstring& modelFile,
                       SLSkybox*       skybox                 = nullptr,
                       SLbool          deleteTexImgAfterBuild = false,
                       SLbool          loadMeshesOnly         = true,
                       SLMaterial*     overrideMat            = nullptr,
                       float           ambientFactor          = 0.5f,
                       SLbool          forceCookTorranceRM    = false);

    //! Add generic GLSL program with shader files to load
    void addProgramGenericToLoad(SLGLProgram*&   program,
                                 const SLstring& vertShaderFile,
                                 const SLstring& fragShaderFile);

    //! Add skybox with HDR texture to load
    void addSkyboxToLoad(SLSkybox*&      skybox,
                         const SLstring& hdrImageWithFullPath,
                         SLVec2i         resolution,
                         SLstring        name);

    //! Add skybox with 6 textures for a cubemap to load
    void addSkyboxToLoad(SLSkybox*&      skybox,
                         const SLstring& cubeMapXPos,
                         const SLstring& cubeMapXNeg,
                         const SLstring& cubeMapYPos,
                         const SLstring& cubeMapYNeg,
                         const SLstring& cubeMapZPos,
                         const SLstring& cubeMapZNeg);

    //! Add generic task
    void addLoadTask(SLAssetLoadTask task);

    void loadAssetsSync();
    void loadAssetsAsync(function<void()> onDone);
    void checkIfAsyncLoadingIsDone();

public:
    SLScene*         _scene;
    SLAssetManager*  _am;
    SLstring         _modelPath;
    SLstring         _texturePath;
    SLstring         _shaderPath;
    SLstring         _fontPath;
    SLVAssetLoadTask _loadTasks;
    function<void()> _onDoneLoading; //!< Callback after threaded loading

    thread             _worker;         //!< worker thread for parallel loading
    atomic<State>      _state;          //!< current state (used for communication between threads)
    mutex              _messageMutex;   //!< mutex protecting state between threads
    condition_variable _messageCondVar; //!< mutex for waiting until state has changed
};
//-----------------------------------------------------------------------------

#endif