/**
 * \file      AppDemoScenePTMuttenzerBox2.h
 * \brief     Class declaration for an SLScene inherited class
 * \details   For more info about App framework and the scene assembly see:
 *            https://cpvrlab.github.io/SLProject4/app-framework.html
 * \date      September 2025
 * \authors   Marcus Hudritsch, Marino von Wattenwyl
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
 */

#ifndef APPDEMOSCENEPTMUTTENZERBOX2_H
#define APPDEMOSCENEPTMUTTENZERBOX2_H

#include <SLScene.h>

//-----------------------------------------------------------------------------
//! Class for the Muttenzer Box path tracing scene with soft gloss materials
/*! The same box and the same two spheres as AppDemoScenePTMuttenzerBox, with
one difference: the shininess of the mirror sphere and both the translucency
and the shininess of the glass sphere are 100 instead of SLMaterial::PERFECT
(1000). Below that limit the path tracer draws the direction from the Phong
lobe of the exponent (SLRay::reflectMC and SLRay::refractMC) instead of taking
the perfect specular one, so the mirror sphere reflects blurred and the glass
sphere is frosted in its refraction as well as in its surface reflection. The
glass sphere is also given a red transmissive color, which makes its caustic on
the floor red. Only the path tracer shows any of this; the OpenGL preview and
the ray tracer render the scene exactly like the original.*/
class AppDemoScenePTMuttenzerBox2 : public SLScene
{
public:
    AppDemoScenePTMuttenzerBox2();

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
    SLGLTexture* _tex1;
    SLGLProgram* _spRefl;
    SLGLProgram* _spRefr;
};
//-----------------------------------------------------------------------------

#endif
