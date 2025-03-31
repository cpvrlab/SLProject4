/**
 * \file      AppDemoSceneErlebARBielCBB.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see: 
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      May 2024
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPDEMOSCENEERLEBAR_BIELCBB_H
#define APPDEMOSCENEERLEBAR_BIELCBB_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for ErlebAR model in Biel Campus Biel-Bienne (CBB).
class AppDemoSceneErlebARBielCBB : public SLScene
{
public:
    AppDemoSceneErlebARBielCBB();
    
    //! All scene specific assets have to be registered for async loading in here.
    /*! @remark All scene sspecific assets have to be loaded async by overriding 
    SLScene::registerAssetsToLoad and SLScene::assemble. Async loading and 
    assembling means that it happens in a parallel thread and that in there are 
    no OpenGL calls allowed. OpenGL calls are only allowed in the main thread.*/
    void registerAssetsToLoad(SLAssetLoader& al) override;

    //! After parallel loading of the assets the scene gets assembled in here.
    /*! @remark All scene-specific assets have to be loaded async by overriding 
    SLScene::registerAssetsToLoad and SLScene::assemble. Async loading and 
    assembling means that it happens in a parallel thread and that in there 
    are no OpenGL calls allowed. OpenGL calls are only allowed in the main 
    thread. It is important that all object instantiations within 
    SLScene::assemble do NOT call any OpenGL functions (gl*) because they happen 
    in a parallel thread. All objects that get rendered have to do their 
    initialization when they are used the first time during rendering in the 
    main thread.*/
    void assemble(SLAssetManager* am, SLSceneView* sv) override;

private:
    SLNode*      _cbb;
    SLGLTexture* _cubemap;
    SLGLProgram* _spRefr;
    SLGLProgram* _spVideoBackground;
};
//-----------------------------------------------------------------------------

#endif
