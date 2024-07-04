/**
 * \file      SLSkybox
 * \authors   Marcus Hudritsch
 * \date      December 2017
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLBox.h>
#include <SLGLProgramGeneric.h>
#include <SLGLTexture.h>
#include <SLGLTextureIBL.h>
#include <SLMaterial.h>
#include <SLSceneView.h>
#include <SLSkybox.h>

//-----------------------------------------------------------------------------
/**
 * @brief Construct a new SLSkybox::SLSkybox object with 6 images for all sides
 * @details It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within a SLScene::assemble function. All objects that get rendered 
 * have to do their OpenGL initialization when they are used the first time 
 * during rendering in the main thread.
 * @param assetMgr Asset manager that will own the skybox mesh
 * @param shaderPath Path to the shader files
 * @param cubeMapXPos Texture image file for the positive X side
 * @param cubeMapXNeg Texture image file for the negative X side
 * @param cubeMapYPos Texture image file for the positive Y side
 * @param cubeMapYNeg Texture image file for the negative Y side
 * @param cubeMapZPos Texture image file for the positive Z side 
 * @param cubeMapZNeg Texture image file for the negative Z side
 * @param name Name of the skybox
 */
SLSkybox::SLSkybox(SLAssetManager* assetMgr,
                   SLstring        shaderPath,
                   SLstring        cubeMapXPos,
                   SLstring        cubeMapXNeg,
                   SLstring        cubeMapYPos,
                   SLstring        cubeMapYNeg,
                   SLstring        cubeMapZPos,
                   SLstring        cubeMapZNeg,
                   SLstring        name) : SLNode(name)
{
    assert(assetMgr &&
           "SLSkybox: asset manager is currently mandatory for sky-boxes! "
           "Alternatively the live-time of the box has to be managed in the sky-box!");

    // Set HDR flag to false, this is a normal SkyBox
    _isHDR              = false;
    _isBuilt            = true;
    _environmentCubemap = nullptr;
    _irradianceCubemap  = nullptr;
    _roughnessCubemap   = nullptr;
    _brdfLutTexture     = nullptr;
    _hdrTexture         = nullptr;
    _exposure           = 1.0f;

    // Create texture, material and program
    _environmentCubemap = new SLGLTexture(assetMgr,
                                          cubeMapXPos,
                                          cubeMapXNeg,
                                          cubeMapYPos,
                                          cubeMapYNeg,
                                          cubeMapZPos,
                                          cubeMapZNeg);

    SLMaterial* matCubeMap = new SLMaterial(assetMgr, "matCubeMap");
    matCubeMap->addTexture(_environmentCubemap);

    SLGLProgram* sp = new SLGLProgramGeneric(assetMgr,
                                             shaderPath + "SkyBox.vert",
                                             shaderPath + "SkyBox.frag");
    matCubeMap->program(sp);

    // Create a box with max. point at min. parameter and vice versa.
    // Like this the boxes normals will point to the inside.
    addMesh(new SLBox(assetMgr,
                      10,
                      10,
                      10,
                      -10,
                      -10,
                      -10,
                      "box",
                      matCubeMap));
}
//-----------------------------------------------------------------------------
//! Constructor for a cube map skybox from a HDR Image
/*! This constructor generates a cube map skybox from a HDR Image and also
all the textures needed for image based lighting and store them in the textures
of the material of this sky box.
*/

/**
 * @brief Construct a new SLSkybox::SLSkybox object with an HDR image
 * @details This constructor generates a cube map skybox from a HDR Image and also
 * all the textures needed for image based lighting and store them in the textures
 * of the material of this sky box.\n
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * @param assetMgr Asset manager that will own the skybox mesh
 * @param shaderPath Path to the shader files
 * @param hdrImage HDR texture image file
 * @param resolution Resolution of the texture objects for image based lighting
 * @param name Name of the skybox
 */
SLSkybox::SLSkybox(SLAssetManager* am,
                   SLstring        shaderPath,
                   SLstring        hdrImage,
                   SLVec2i         resolution,
                   SLstring        name) : SLNode(name)
{
    // Set HDR flag to true, this is a HDR SkyBox
    _isHDR    = true;
    _isBuilt  = false;
    _exposure = 1.0f;

    // Create shader program for the background
    SLGLProgram* backgroundShader = new SLGLProgramGeneric(am,
                                                           shaderPath + "PBR_SkyboxHDR.vert",
                                                           shaderPath + "PBR_SkyboxHDR.frag");

    // The HDR texture is only used to create the environment,
    // the irradiance and the roughness cubemaps
    _hdrTexture = new SLGLTexture(am,
                                  hdrImage,
                                  GL_LINEAR,
                                  GL_LINEAR,
                                  TT_hdr,
                                  GL_CLAMP_TO_EDGE,
                                  GL_CLAMP_TO_EDGE);

    // The environment cubemap is used for the rendering of the skybox
    _environmentCubemap = new SLGLTextureIBL(am,
                                             shaderPath,
                                             _hdrTexture,
                                             resolution,
                                             TT_environmentCubemap,
                                             GL_TEXTURE_CUBE_MAP,
                                             true, // read back environment for ray tracing
                                             GL_LINEAR_MIPMAP_LINEAR);

    // The irradiance cubemap is used for the ambient indirect light of PBR materials
    _irradianceCubemap = new SLGLTextureIBL(am,
                                            shaderPath,
                                            _environmentCubemap,
                                            SLVec2i(32, 32),
                                            TT_irradianceCubemap,
                                            GL_TEXTURE_CUBE_MAP);

    // The roughness cubemap is used for the blurred reflections on PBR materials
    _roughnessCubemap = new SLGLTextureIBL(am,
                                           shaderPath,
                                           _environmentCubemap,
                                           SLVec2i(128, 128),
                                           TT_roughnessCubemap,
                                           GL_TEXTURE_CUBE_MAP);

    // The BRDF lookup texture is used for the specular gloss reflection calculation
    _brdfLutTexture = new SLGLTextureIBL(am,
                                         shaderPath,
                                         nullptr,
                                         SLVec2i(512, 512),
                                         TT_brdfLUT,
                                         GL_TEXTURE_2D);

    // Create the material of the sky box and store there the other texture to be used for other materials
    SLMaterial* envMaterial = new SLMaterial(am, "matCubeMap");
    envMaterial->addTexture(_environmentCubemap);
    envMaterial->program(backgroundShader);
    envMaterial->skybox(this);

    // Create the box for the sky box
    addMesh(new SLBox(am,
                      10,
                      10,
                      10,
                      -10,
                      -10,
                      -10,
                      "box",
                      envMaterial));
}

//-----------------------------------------------------------------------------
//! Builds all texture for HDR image based lighting
void SLSkybox::build()
{
    _hdrTexture->build(0);
    _environmentCubemap->build(0);
    _irradianceCubemap->build(2);
    _roughnessCubemap->build(3);
    _brdfLutTexture->build(4);
    _isBuilt = true;
}
//-----------------------------------------------------------------------------
//! Draw the skybox with a cube map with the camera in its center.
void SLSkybox::drawAroundCamera(SLSceneView* sv)
{
    assert(sv && "No SceneView passed to SLSkybox::drawAroundCamera");

    SLGLState* stateGL = SLGLState::instance();

    if (_isHDR && !_isBuilt)
        build();

    // Put skybox at the cameras position
    translation(sv->camera()->translationWS());

    // Apply world transform
    stateGL->modelMatrix = updateAndGetWM();

    // Freeze depth buffer
    stateGL->depthMask(false);

    // Change depth buffer comparisons for HDR SkyBoxes
    if (_isHDR)
        stateGL->depthFunc(GL_LEQUAL);

    // Draw the box
    drawMesh(sv);

    // Change back the depth buffer comparisons
    stateGL->depthFunc(GL_LESS);

    // Unlock depth buffer
    stateGL->depthMask(true);
}
//-----------------------------------------------------------------------------
//! Returns the color in the skybox at the the specified direction dir
SLCol4f SLSkybox::colorAtDir(const SLVec3f& dir)
{
    if (_mesh && _mesh->mat())
    {
        SLMaterial* mat = _mesh->mat();

        if (!mat->textures(TT_diffuse).empty())
        {
            if (mat->textures(TT_diffuse)[0]->images().size() == 6)
            {
                SLGLTexture* tex = mat->textures(TT_diffuse)[0];
                return tex->getTexelf(dir);
            }
        }

        if (!mat->textures(TT_environmentCubemap).empty())
        {
            if (mat->textures(TT_environmentCubemap)[0]->images().size() == 6)
            {
                SLGLTexture* tex = mat->textures(TT_environmentCubemap)[0];
                return tex->getTexelf(dir);
            }
        }
    }

    return SLCol4f::BLACK;
}
//-----------------------------------------------------------------------------
