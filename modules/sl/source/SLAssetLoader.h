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

using namespace std;

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

    // Getters
    bool isLoading() const { return _isLoading; }
    void scene(SLScene* scene) { _scene = scene; }

    //! add 2D textures with internal image allocation
    void addTextureToLoad(SLGLTexture*& texture,
                          SLstring      imageFilename,
                          SLint         min_filter = GL_LINEAR_MIPMAP_LINEAR,
                          SLint         mag_filter = GL_LINEAR,
                          SLTextureType type       = TT_unknown,
                          SLint         wrapS      = GL_REPEAT,
                          SLint         wrapT      = GL_REPEAT);

    //! add cube map texture with internal image allocation
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

    void addNodeToLoad(SLNode*&    node,
                       SLstring    path,
                       SLSkybox*   skybox                 = nullptr,
                       SLbool      deleteTexImgAfterBuild = false,
                       SLbool      loadMeshesOnly         = true,
                       SLMaterial* overrideMat            = nullptr,
                       float       ambientFactor          = 0.5f,
                       SLbool      forceCookTorranceRM    = false);

    void addProgramGenericToLoad(SLGLProgram*&   program,
                                 const SLstring& vertShaderFile,
                                 const SLstring& fragShaderFile);

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