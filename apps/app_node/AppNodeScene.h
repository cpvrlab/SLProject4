/**
 * \file      AppNodeScene.h
 * \brief     Scene creation callback for the the node demo app
 * \date      June 2024
 * \authors   Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef APPNODESCENE_H
#define APPNODESCENE_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
class AppNodeScene : public SLScene
{
public:
    AppNodeScene();
    
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
    SLNode* _axesNode;
};
//-----------------------------------------------------------------------------

#endif
