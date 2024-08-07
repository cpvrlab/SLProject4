/**
 * \file      SLLight.cpp
 * \date      July 2014
 * \authors   Marcus Hudritsch
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#include <SLLight.h>
#include <SLShadowMap.h>

//-----------------------------------------------------------------------------
SLCol4f SLLight::globalAmbient    = SLCol4f(0.1f, 0.1f, 0.1f, 1.0f);
SLfloat SLLight::gamma            = 1.0f;
SLbool  SLLight::doColoredShadows = false;
//-----------------------------------------------------------------------------
/**
 * @brief Construct a new SLLight::SLLight object
 * @remarks It is important that during instantiation NO OpenGL functions (gl*) 
 * get called because this constructor will be most probably called in a parallel 
 * thread from within an SLScene::registerAssetsToLoad or SLScene::assemble 
 * function. All objects that get rendered have to do their OpenGL initialization 
 * when they are used the first time during rendering in the main thread.
 * @param ambiPower Ambient light power
 * @param diffPower Diffuse light power
 * @param specPower Specular light power
 * @param id Light ID
 */
SLLight::SLLight(SLfloat ambiPower,
                 SLfloat diffPower,
                 SLfloat specPower,
                 SLint   id)
{
    // Set parameter of SLLight
    _id               = id;
    _isOn             = true;
    _spotCutOffDEG    = 180.0f;
    _spotCosCutOffRAD = cos(Utils::DEG2RAD * _spotCutOffDEG);
    _spotExponent     = 1.0f;
    _createsShadows   = false;
    _shadowMap        = nullptr;
    _doSoftShadows    = false;
    _softShadowLevel  = 1;
    _shadowMinBias    = 0.001f;
    _shadowMaxBias    = 0.008f;

    // Set parameters of inherited SLMaterial
    _ambientColor.set(1, 1, 1);
    _ambientPower = ambiPower;
    _diffuseColor.set(1, 1, 1);
    _diffusePower = diffPower;
    _specularColor.set(1, 1, 1);
    _specularPower = specPower;

    // By default, there is no attenuation set. This is physically not correct
    // Default OpenGL:      kc=1, kl=0, kq=0
    // Physically correct:  kc=0, kl=0, kq=1
    // set quadratic attenuation with d = distance to light
    //                      1
    // attenuation = ------------------
    //               kc + kl*d + kq*d*d

    // init values to avoid used of uninitialized values
    _kc=1.0f;
    _kl=0.0f;
    _kq=0.0f;

    kc(1.0f);
    kl(0.0f);
    kq(0.0f);
}
//-----------------------------------------------------------------------------
void SLLight::kc(SLfloat kc)
{
    _kc           = kc;
    _isAttenuated = !(_kc == 1.0f && _kl == 0.0f && _kq == 0.0f);
}
//-----------------------------------------------------------------------------
void SLLight::kl(SLfloat kl)
{
    _kl           = kl;
    _isAttenuated = !(_kc == 1.0f && _kl == 0.0f && _kq == 0.0f);
}
//-----------------------------------------------------------------------------
void SLLight::kq(SLfloat kq)
{
    _kq           = kq;
    _isAttenuated = !(_kc == 1.0f && _kl == 0.0f && _kq == 0.0f);
}
//-----------------------------------------------------------------------------
void SLLight::spotCutOffDEG(const SLfloat cutOffAngleDEG)
{
    _spotCutOffDEG    = cutOffAngleDEG;
    _spotCosCutOffRAD = cos(Utils::DEG2RAD * _spotCutOffDEG);
}
//-----------------------------------------------------------------------------
void SLLight::createsShadows(SLbool createsShadows)
{
    _createsShadows = createsShadows;
}
//-----------------------------------------------------------------------------
//! SLLight::renderShadowMap renders the shadow map of the light
void SLLight::renderShadowMap(SLSceneView* sv, SLNode* root)
{
    // Check if no shadow map was created at load time
    if (!_shadowMap)
    {
        this->createShadowMap();
    }

    _shadowMap->renderShadows(sv, root);
}
//-----------------------------------------------------------------------------
