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

#include <condition_variable>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <optional>

#include <SL.h>
#include <SLGLTexture.h>;

class SLScene;
class SLAssetManager;
class SLNode;
class SLSkybox;
class SLMaterial;

using std::atomic;
using std::function;
using std::optional;
using std::thread;

//-----------------------------------------------------------------------------
typedef function<void()>        SLAssetLoadTask;
typedef vector<SLAssetLoadTask> SLVAssetLoadTask;
//-----------------------------------------------------------------------------
class SLAssetLoader
{
public:
    SLAssetLoader(SLstring modelPath,
                  SLstring texturePath,
                  SLstring shaderPath);
    ~SLAssetLoader();

    // Setters
    void scene(SLScene* scene) { _scene = scene; }

    // Getters
    bool     isLoading() const { return _isLoading; }
    SLstring modelPath() const { return _modelPath; }
    SLstring shaderPath() const { return _shaderPath; }
    SLstring texturePath() const { return _texturePath; }

    //! Add 2D textures with internal image allocation
    void addTextureToLoad(SLGLTexture*& texture,
                          SLstring      imageFilename,
                          SLint         min_filter = GL_LINEAR_MIPMAP_LINEAR,
                          SLint         mag_filter = GL_LINEAR,
                          SLTextureType type       = TT_unknown,
                          SLint         wrapS      = GL_REPEAT,
                          SLint         wrapT      = GL_REPEAT);

    //! Add cube map texture with internal image allocation
    void addTextureToLoad(SLGLTexture*&   texture,
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

      //! Add mesh from file to load via assimp loader
      void addNodeToLoad(SLNode*&    node,
                         SLstring    path,
                         SLSkybox*   skybox                 = nullptr,
                         SLbool      deleteTexImgAfterBuild = false,
                         SLbool      loadMeshesOnly         = true,
                         SLMaterial* overrideMat            = nullptr,
                         float       ambientFactor          = 0.5f,
                         SLbool      forceCookTorranceRM    = false);

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

    void loadAll(function<void()> onDone);
    void update();

private:
    SLScene*         _scene;
    SLAssetManager*  _am;
    SLstring         _modelPath;
    SLstring         _texturePath;
    SLstring         _shaderPath;
    SLVAssetLoadTask _loadTasks;
    bool             _isLoading;
    atomic<bool>     _isDone;        //!< thread save boolean for checking parallel loading
    optional<thread> _worker;        //!< worker thread for parallel loading
    function<void()> _onDoneLoading; //!< Callback after threaded loading
};
//-----------------------------------------------------------------------------

#endif