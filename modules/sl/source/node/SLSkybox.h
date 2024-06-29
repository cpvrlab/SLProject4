/**
 * \file      SLSkybox
 * \authors   Marcus Hudritsch
 * \date      December 2017
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifndef SLSKYBOX_H
#define SLSKYBOX_H

#include <SLEnums.h>
#include <SLNode.h>

class SLSceneView;
class SLAssetManager;

//-----------------------------------------------------------------------------
//! Skybox node class with a SLBox mesh
/*! The skybox instance is a node with a SLBox mesh with inwards pointing
normals. It gets drawn in SLSceneView::draw3DGL with frozen depth buffer and a
special cubemap shader. The box is always with the active camera in its
center. It has to be created in SLScene::onLoad and assigned to the skybox
pointer of SLSceneView. See the Skybox shader example.
*/
class SLSkybox : public SLNode
{
public:
    SLSkybox(SLAssetManager* assetMgr,
             SLstring        shaderPath,
             SLstring        cubeMapXPos,
             SLstring        cubeMapXNeg,
             SLstring        cubeMapYPos,
             SLstring        cubeMapYNeg,
             SLstring        cubeMapZPos,
             SLstring        cubeMapZNeg,
             SLstring        name = "Default Skybox");

    SLSkybox(SLAssetManager* am,
             SLstring        shaderPath,
             SLstring        hdrImage,
             SLVec2i         resolution,
             SLstring        name = "HDR Skybox");

    ~SLSkybox() { ; }

    // Getters
    SLGLTexture* environmentCubemap() { return _environmentCubemap; }
    SLGLTexture* irradianceCubemap() { return _irradianceCubemap; }
    SLGLTexture* roughnessCubemap() { return _roughnessCubemap; }
    SLGLTexture* brdfLutTexture() { return _brdfLutTexture; }
    SLfloat      exposure() { return _exposure; }
    SLbool       isHDR() { return _isHDR; }

    // Setters
    void exposure(float exp) { _exposure = exp; }

    void    drawAroundCamera(SLSceneView* sv);
    SLCol4f colorAtDir(const SLVec3f& dir);

private:
    void         build();
    SLGLTexture* _environmentCubemap;
    SLGLTexture* _irradianceCubemap;
    SLGLTexture* _roughnessCubemap;
    SLGLTexture* _brdfLutTexture;
    SLGLTexture* _hdrTexture;
    SLfloat      _exposure;
    SLbool       _isHDR;   //!< flag for HDR skyboxes
    SLbool       _isBuilt; //!< flag for late HDR skybox building
};
//-----------------------------------------------------------------------------
#endif //#define SLSKYBOX_H
