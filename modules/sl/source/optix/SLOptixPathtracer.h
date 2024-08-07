/**
 * \file      SLOptixPathtracer.h
 * \authors   Nic Dorner
 * \date      October 2019
 * \authors   Nic Dorner
 * \copyright http://opensource.org/licenses/GPL-3.0
 * \remarks   Please use clangformat to format the code. See more code style on
 *            https://github.com/cpvrlab/SLProject4/wiki/SLProject-Coding-Style
*/

#ifdef SL_HAS_OPTIX
#    ifndef SLOPTIXPATHTRACER_H
#        define SLOPTIXPATHTRACER_H
#        include <SLOptixRaytracer.h>
#        include <curand_kernel.h>

class SLScene;
class SLSceneView;
class SLRay;
class SLMaterial;
class SLCamera;

//-----------------------------------------------------------------------------
class SLOptixPathtracer : public SLOptixRaytracer
{
public:
    SLOptixPathtracer();
    ~SLOptixPathtracer();

    // setup path tracer
    void setupOptix() override;
    void setupScene(SLSceneView* sv, SLAssetManager* am) override;
    void updateScene(SLSceneView* sv) override;

    // path tracer functions
    SLbool render();

    SLbool getDenoiserEnabled() const { return _denoiserEnabled; }
    SLint  samples() const { return _samples; }

    void setDenoiserEnabled(SLbool denoiserEnabled) { _denoiserEnabled = denoiserEnabled; }
    void samples(SLint samples) { _samples = samples; }

    SLfloat denoiserMS() const { return _denoiserMS; }

protected:
    SLint                          _samples;
    SLOptixCudaBuffer<curandState> _curandBuffer = SLOptixCudaBuffer<curandState>();

private:
    OptixDenoiser           _optixDenoiser;
    OptixDenoiserSizes      _denoiserSizes;
    SLOptixCudaBuffer<void> _denoserState;
    SLOptixCudaBuffer<void> _scratch;

    // Settings
    SLbool  _denoiserEnabled = true;
    SLfloat _denoiserMS; //!< Denoiser time in ms
};
//-----------------------------------------------------------------------------
#    endif // SLOPTIXPATHTRACER_H
#endif     // SL_HAS_OPTIX
