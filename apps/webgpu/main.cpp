#if defined(_WIN32)
#    define SYSTEM_WINDOWS
#    define GLFW_EXPOSE_NATIVE_WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#elif defined(__linux__)
#    define SYSTEM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11
#elif defined(__APPLE__)
#    define SYSTEM_DARWIN
#    define GLFW_EXPOSE_NATIVE_COCOA
#    define GLFW_NATIVE_INCLUDE_NONE

#    define OBJC_OLD_DISPATCH_PROTOTYPES 1
#    include <objc/objc.h>
#    include <objc/message.h>
#    include <objc/runtime.h>
#    include <ApplicationServices/ApplicationServices.h>
#endif

#include <webgpu.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <SLMat4.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#define WEBGPU_DEMO_LOG(msg) std::cout << (msg) << std::endl

#define WEBGPU_DEMO_CHECK(condition, errorMsg) \
    if (!(condition)) \
    { \
        std::cerr << (errorMsg) << std::endl; \
        std::exit(1); \
    }

struct alignas(16) ShaderUniformData
{
    float projectionMatrix[16];
    float viewMatrix[16];
    float modelMatrix[16];
};

static_assert(sizeof(ShaderUniformData) % 16 == 0, "uniform data size must be a multiple of 16");

void handleAdapterRequest(WGPURequestAdapterStatus status,
                          WGPUAdapter              adapter,
                          char const*              message,
                          void*                    userdata)
{
    WEBGPU_DEMO_CHECK(status == WGPURequestAdapterStatus_Success,
                      "[WebGPU] Failed to acquire adapter: " + std::string(message));
    WEBGPU_DEMO_LOG("[WebGPU] Adapter acquired");

    WGPUAdapter* outAdapter = (WGPUAdapter*)userdata;
    *outAdapter             = adapter;
}

void handleDeviceRequest(WGPURequestDeviceStatus status,
                         WGPUDevice              device,
                         char const*             message,
                         void*                   userdata)
{
    WEBGPU_DEMO_CHECK(status == WGPURequestDeviceStatus_Success,
                      "[WebGPU] Failed to acquire device: " + std::string(message));
    WEBGPU_DEMO_LOG("[WebGPU] Device acquired");

    WGPUDevice* outDevice = (WGPUDevice*)userdata;
    *outDevice            = device;
}

int main(int argc, const char* argv[])
{
    // === Initialize GLFW ===

    WEBGPU_DEMO_CHECK(glfwInit(), "[GLFW] Failed to initialize");
    WEBGPU_DEMO_LOG("[GLFW] Initialized");

    // === Create the GLFW window ===

    // Prevent GLFW from creating an OpenGL context as the underlying graphics API probably won't be OpenGL.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "WebGPU Demo", nullptr, nullptr);
    WEBGPU_DEMO_CHECK(window, "[GLFW] Window created");

    // === Create a WebGPU instance ===
    // The instance is the root interface to WebGPU through which we create all other WebGPU resources.

    WGPUInstance instance = wgpuCreateInstance(nullptr);
    WEBGPU_DEMO_CHECK(instance, "[WebGPU] Failed to create instance");
    WEBGPU_DEMO_LOG("[WebGPU] Instance created");

    // === Acquire a WebGPU adapter ===
    // An adapter provides information about the capabilities of the GPU.

    WGPURequestAdapterOptions adapterOptions = {};

    WGPUAdapter adapter;
    wgpuInstanceRequestAdapter(instance, &adapterOptions, handleAdapterRequest, &adapter);

    WGPUSupportedLimits adapterLimits;
    wgpuAdapterGetLimits(adapter, &adapterLimits);

    // === Acquire a WebGPU device ===
    // A device provides access to a GPU and is created from an adapter.
    // We specify the capabilites that we require our device to have in requiredLimits.
    // We cannot access more resources than specified in the required limits,
    // which is how WebGPU prevents code from working on one machine and not on another.

    WGPURequiredLimits requiredLimits                      = {};
    requiredLimits.limits.maxVertexAttributes              = 2u;
    requiredLimits.limits.maxVertexBuffers                 = 1u;
    requiredLimits.limits.maxBufferSize                    = 1024ull;
    requiredLimits.limits.maxVertexBufferArrayStride       = 5u * sizeof(float);
    requiredLimits.limits.maxInterStageShaderComponents    = 2u;
    requiredLimits.limits.maxBindGroups                    = 1u;
    requiredLimits.limits.maxBindingsPerBindGroup          = 3u;
    requiredLimits.limits.maxUniformBuffersPerShaderStage  = 1u;
    requiredLimits.limits.maxUniformBufferBindingSize      = 512ull;
    requiredLimits.limits.maxSampledTexturesPerShaderStage = 1u;
    requiredLimits.limits.maxSamplersPerShaderStage        = 1u;
    requiredLimits.limits.maxTextureDimension1D            = 4096;
    requiredLimits.limits.maxTextureDimension2D            = 4096;
    requiredLimits.limits.maxTextureArrayLayers            = 1;
    requiredLimits.limits.minStorageBufferOffsetAlignment  = adapterLimits.limits.minStorageBufferOffsetAlignment;
    requiredLimits.limits.minUniformBufferOffsetAlignment  = adapterLimits.limits.minUniformBufferOffsetAlignment;

    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.label                = "Demo Device";
    deviceDesc.requiredLimits       = &requiredLimits;
    deviceDesc.defaultQueue.label   = "Demo Queue";

    WGPUDevice device;
    wgpuAdapterRequestDevice(adapter, &deviceDesc, handleDeviceRequest, &device);

    // === Acquire a WebGPU queue ===
    // The queue is where the commands for the GPU are submitted to.

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    WEBGPU_DEMO_CHECK(queue, "[WebGPU] Failed to acquire queue");
    WEBGPU_DEMO_LOG("[WebGPU] Queue acquired");

    // === Create a WebGPU surface ===
    // The surface is where our rendered images will be presented to.
    // It is created from window handles on most platforms and from a canvas in the browser.

#if defined(SYSTEM_WINDOWS)
    WGPUSurfaceDescriptorFromWindowsHWND nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                          = WGPUSType_SurfaceDescriptorFromWindowsHWND;
    nativeSurfaceDesc.hinstance                            = GetModuleHandle(nullptr);
    nativeSurfaceDesc.hwnd                                 = glfwGetWin32Window(window);
#elif defined(SYSTEM_LINUX)
    WGPUSurfaceDescriptorFromXlibWindow nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                         = WGPUSType_SurfaceDescriptorFromXlibWindow;
    nativeSurfaceDesc.display                             = glfwGetX11Display();
    nativeSurfaceDesc.window                              = glfwGetX11Window(window);
#elif defined(SYSTEM_DARWIN)
    id cocoaWindow = glfwGetCocoaWindow(window);

    id contentView = objc_msgSend(cocoaWindow, sel_registerName("contentView"));
    objc_msgSend(contentView, sel_getUid("setWantsLayer:"), 1);

    objc_class* metalLayerClass = objc_getClass("CAMetalLayer");
    id          metalLayer      = objc_msgSend((id)metalLayerClass, sel_getUid("layer"));
    objc_msgSend(contentView, sel_registerName("setLayer:"), metalLayer);

    WGPUSurfaceDescriptorFromMetalLayer nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                         = WGPUSType_SurfaceDescriptorFromMetalLayer;
    nativeSurfaceDesc.layer                               = metalLayer;
#endif

    WGPUSurfaceDescriptor surfaceDesc = {};
    surfaceDesc.label                 = "Demo Surface";
    surfaceDesc.nextInChain           = (const WGPUChainedStruct*)&nativeSurfaceDesc;

    WGPUSurface surface = wgpuInstanceCreateSurface(instance, &surfaceDesc);
    WEBGPU_DEMO_CHECK(surface, "[WebGPU] Failed to create surface");
    WEBGPU_DEMO_LOG("[WebGPU] Surface created");

    // === Configure the surface ===
    // The surface needs to be configured before images can be presented.

    // Query the surface capabilities from the adapter.
    WGPUSurfaceCapabilities surfaceCapabilities;
    wgpuSurfaceGetCapabilities(surface, adapter, &surfaceCapabilities);

    // Get the window size from the GLFW window.
    int surfaceWidth;
    int surfaceHeight;
    glfwGetWindowSize(window, &surfaceWidth, &surfaceHeight);

    WGPUSurfaceConfiguration surfaceConfig = {};
    surfaceConfig.device                   = device;
    surfaceConfig.usage                    = WGPUTextureUsage_RenderAttachment;
    surfaceConfig.format                   = surfaceCapabilities.formats[0];
    surfaceConfig.presentMode              = WGPUPresentMode_Mailbox;
    surfaceConfig.alphaMode                = surfaceCapabilities.alphaModes[0];
    surfaceConfig.width                    = surfaceWidth;
    surfaceConfig.height                   = surfaceHeight;
    wgpuSurfaceConfigure(surface, &surfaceConfig);
    WEBGPU_DEMO_LOG("[WebGPU] Surface configured");

    // === Create the vertex buffer ===
    // The vertex buffer contains the input data for the shader.

    // clang-format off
    std::vector<float> vertexData =
    {
		// left
		-0.5,  0.5, -0.5,    0.0f, 0.0f,
		-0.5, -0.5, -0.5,    0.0f, 1.0f,
		-0.5,  0.5,  0.5,    1.0f, 0.0f,
		-0.5,  0.5,  0.5,    1.0f, 0.0f,
		-0.5, -0.5, -0.5,    0.0f, 1.0f,
		-0.5, -0.5,  0.5,    1.0f, 1.0f,

		// right
		 0.5,  0.5,  0.5,    0.0f, 0.0f,
		 0.5, -0.5,  0.5,    0.0f, 1.0f,
		 0.5,  0.5, -0.5,    1.0f, 0.0f,
		 0.5,  0.5, -0.5,    1.0f, 0.0f,
		 0.5, -0.5,  0.5,    0.0f, 1.0f,
		 0.5, -0.5, -0.5,    1.0f, 1.0f,

		// bottom
		-0.5, -0.5,  0.5,    0.0f, 0.0f,
		-0.5, -0.5, -0.5,    0.0f, 1.0f,
		 0.5, -0.5,  0.5,    1.0f, 0.0f,
		 0.5, -0.5,  0.5,    1.0f, 0.0f,
		-0.5, -0.5, -0.5,    0.0f, 1.0f,
		 0.5, -0.5, -0.5,    1.0f, 1.0f,

	    // top
        -0.5,  0.5, -0.5,    0.0f, 0.0f,
		-0.5,  0.5,  0.5,    0.0f, 1.0f,
		 0.5,  0.5, -0.5,    1.0f, 0.0f,
		 0.5,  0.5, -0.5,    1.0f, 0.0f,
		-0.5,  0.5,  0.5,    0.0f, 1.0f,
		 0.5,  0.5,  0.5,    1.0f, 1.0f,

		// back
		 0.5,  0.5, -0.5,    0.0f, 0.0f,
		 0.5, -0.5, -0.5,    0.0f, 1.0f,
		-0.5,  0.5, -0.5,    1.0f, 0.0f,
		-0.5,  0.5, -0.5,    1.0f, 0.0f,
		 0.5, -0.5, -0.5,    0.0f, 1.0f,
		-0.5, -0.5, -0.5,    1.0f, 1.0f,
	
		// front
		-0.5,  0.5,  0.5,    0.0f, 0.0f,
		-0.5, -0.5,  0.5,    0.0f, 1.0f,
		 0.5,  0.5,  0.5,    1.0f, 0.0f,
		 0.5,  0.5,  0.5,    1.0f, 0.0f,
		-0.5, -0.5,  0.5,    0.0f, 1.0f,
		 0.5, -0.5,  0.5,    1.0f, 1.0f,
    };
    // clang-format on

    unsigned vertexCount    = vertexData.size() / 5;
    unsigned vertexDataSize = vertexData.size() * sizeof(float);

    WGPUBufferDescriptor vertexBufferDesc = {};
    vertexBufferDesc.label                = "Demo Vertex Buffer";
    vertexBufferDesc.size                 = vertexDataSize;
    vertexBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;

    WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(device, &vertexBufferDesc);
    WEBGPU_DEMO_CHECK(vertexBuffer, "[WebGPU] Failed to create vertex buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Vertex buffer created");

    // Upload the data to the GPU.
    wgpuQueueWriteBuffer(queue, vertexBuffer, 0, vertexData.data(), vertexDataSize);

    // === Create the index buffer ===

    std::vector<std::uint16_t> indexData = {};
    for (std::uint16_t index = 0; index < 36; index++)
        indexData.push_back(index);

    unsigned indexCount    = indexData.size();
    unsigned indexDataSize = indexData.size() * sizeof(std::uint16_t);

    WGPUBufferDescriptor indexBufferDesc = {};
    indexBufferDesc.label                = "Demo Index Buffer";
    indexBufferDesc.size                 = indexDataSize;
    indexBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;

    WGPUBuffer indexBuffer = wgpuDeviceCreateBuffer(device, &indexBufferDesc);
    WEBGPU_DEMO_CHECK(indexBuffer, "[WebGPU] Failed to create index buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Index buffer created");

    wgpuQueueWriteBuffer(queue, indexBuffer, 0, indexData.data(), indexDataSize);

    // === Create the uniform buffer ===

    WGPUBufferDescriptor uniformBufferDesc = {};
    uniformBufferDesc.label                = "Demo Uniform Buffer";
    uniformBufferDesc.size                 = sizeof(ShaderUniformData);
    uniformBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;

    WGPUBuffer uniformBuffer = wgpuDeviceCreateBuffer(device, &uniformBufferDesc);
    WEBGPU_DEMO_CHECK(indexBuffer, "[WebGPU] Failed to create uniform buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Uniform buffer created");

    ShaderUniformData uniformData = {};
    wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &uniformData, sizeof(ShaderUniformData));

    // === Create the depth texture ===

    WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;

    WGPUTextureDescriptor depthTextureDesc   = {};
    depthTextureDesc.dimension               = WGPUTextureDimension_2D;
    depthTextureDesc.format                  = depthTextureFormat;
    depthTextureDesc.mipLevelCount           = 1;
    depthTextureDesc.sampleCount             = 1;
    depthTextureDesc.size.width              = surfaceWidth;
    depthTextureDesc.size.height             = surfaceHeight;
    depthTextureDesc.size.depthOrArrayLayers = 1;
    depthTextureDesc.usage                   = WGPUTextureUsage_RenderAttachment;
    depthTextureDesc.viewFormatCount         = 1;
    depthTextureDesc.viewFormats             = &depthTextureFormat;

    WGPUTexture depthTexture = wgpuDeviceCreateTexture(device, &depthTextureDesc);
    WEBGPU_DEMO_CHECK(depthTexture, "[WebGPU] Failed to create depth texture");
    WEBGPU_DEMO_LOG("[WebGPU] Depth texture created");

    WGPUTextureViewDescriptor depthTextureViewDesc = {};
    depthTextureViewDesc.aspect                    = WGPUTextureAspect_DepthOnly;
    depthTextureViewDesc.baseArrayLayer            = 0;
    depthTextureViewDesc.arrayLayerCount           = 1;
    depthTextureViewDesc.baseMipLevel              = 0;
    depthTextureViewDesc.mipLevelCount             = 1;
    depthTextureViewDesc.dimension                 = WGPUTextureViewDimension_2D;
    depthTextureViewDesc.format                    = depthTextureFormat;

    WGPUTextureView depthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDesc);
    WEBGPU_DEMO_CHECK(depthTextureView, "[WebGPU] Failed to create depth texture view");
    WEBGPU_DEMO_LOG("[WebGPU] Depth texture view created");

    // === Create the texture ===

    cv::Mat image = cv::imread(std::string(SL_PROJECT_ROOT) + "/data/images/textures/brickwall0512_C.jpg");
    cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);

    unsigned imageWidth    = image.cols;
    unsigned imageHeight   = image.rows;
    unsigned pixelDataSize = 4 * imageWidth * imageHeight;

    WGPUTextureDescriptor textureDesc   = {};
    textureDesc.label                   = "Demo Texture";
    textureDesc.usage                   = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
    textureDesc.dimension               = WGPUTextureDimension_2D;
    textureDesc.size.width              = imageWidth;
    textureDesc.size.height             = imageHeight;
    textureDesc.size.depthOrArrayLayers = 1;
    textureDesc.format                  = WGPUTextureFormat_RGBA8UnormSrgb;
    textureDesc.mipLevelCount           = 1;
    textureDesc.sampleCount             = 1;

    WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDesc);
    WEBGPU_DEMO_CHECK(texture, "[WebGPU] Failed to create texture");
    WEBGPU_DEMO_LOG("[WebGPU] Texture created");

    // Where do we copyu the pixel data to?
    WGPUImageCopyTexture destination = {};
    destination.texture              = texture;
    destination.mipLevel             = 0;
    destination.origin.x             = 0;
    destination.origin.y             = 0;
    destination.origin.z             = 0;
    destination.aspect               = WGPUTextureAspect_All;

    // Where do we copy the pixel data from?
    WGPUTextureDataLayout pixelDataLayout = {};
    pixelDataLayout.offset                = 0;
    pixelDataLayout.bytesPerRow           = 4 * textureDesc.size.width;
    pixelDataLayout.rowsPerImage          = textureDesc.size.height;

    // Upload the data to the GPU.
    wgpuQueueWriteTexture(queue, &destination, image.data, pixelDataSize, &pixelDataLayout, &textureDesc.size);

    // === Create a texture view into the texture ===
    WGPUTextureViewDescriptor textureViewDesc = {};
    textureViewDesc.aspect                    = WGPUTextureAspect_All;
    textureViewDesc.baseArrayLayer            = 0;
    textureViewDesc.arrayLayerCount           = 1;
    textureViewDesc.baseMipLevel              = 0;
    textureViewDesc.mipLevelCount             = 1;
    textureViewDesc.dimension                 = WGPUTextureViewDimension_2D;
    textureViewDesc.format                    = textureDesc.format;

    WGPUTextureView textureView = wgpuTextureCreateView(texture, &textureViewDesc);
    WEBGPU_DEMO_CHECK(textureView, "[WebGPU] Failed to create texture view");
    WEBGPU_DEMO_LOG("[WebGPU] Texture view created");

    // === Create the texture sampler ===
    // The sampler is used to look up values of the texture in the shader.
    // We could look up values directly without a sampler, but with a sampler we
    // get access to features like interpolation and mipmapping.

    WGPUSamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU          = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeV          = WGPUAddressMode_ClampToEdge;
    samplerDesc.addressModeW          = WGPUAddressMode_ClampToEdge;
    samplerDesc.magFilter             = WGPUFilterMode_Linear;
    samplerDesc.minFilter             = WGPUFilterMode_Linear;
    samplerDesc.mipmapFilter          = WGPUMipmapFilterMode_Linear;
    samplerDesc.lodMinClamp           = 0.0f;
    samplerDesc.lodMaxClamp           = 1.0f;
    samplerDesc.compare               = WGPUCompareFunction_Undefined;
    samplerDesc.maxAnisotropy         = 1;

    WGPUSampler sampler = wgpuDeviceCreateSampler(device, &samplerDesc);
    WEBGPU_DEMO_CHECK(sampler, "[WebGPU] Failed to create sampler");
    WEBGPU_DEMO_LOG("[WebGPU] Sampler created");

    // === Create the shader module ===
    // Create a shader module for use in our render pipeline.
    // Shaders are written in a WebGPU-specific language called WGSL (WebGPU shader language).
    // Shader modules can be used in multiple pipeline stages by specifying different entry points.

    const char* shaderSource = R"(
        struct VertexInput {
            @location(0) position: vec3f,
            @location(1) uvs: vec2f,
        };

        struct Uniforms {
            projectionMatrix: mat4x4f,
            viewMatrix: mat4x4f,
            modelMatrix: mat4x4f,
        };

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) uvs: vec2f,
        }
        
        @group(0) @binding(0) var<uniform> uniforms: Uniforms;
        @group(0) @binding(1) var texture: texture_2d<f32>;
        @group(0) @binding(2) var textureSampler: sampler;

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var localPos = vec4f(in.position, 1.0);
            var worldPos = uniforms.modelMatrix * localPos;

            var out: VertexOutput;
            out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
            out.uvs = in.uvs;
            return out;
        }

        @fragment
        fn fs_main(in: VertexOutput) -> @location(0) vec4f {
            return textureSample(texture, textureSampler, in.uvs).rgba;
        }
    )";

    WGPUShaderModuleWGSLDescriptor shaderModuleWGSLDesc = {};
    shaderModuleWGSLDesc.chain.sType                    = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderModuleWGSLDesc.code                           = shaderSource;

    WGPUShaderModuleDescriptor shaderModuleDesc = {};
    shaderModuleDesc.label                      = "Demo Shader";
    shaderModuleDesc.nextInChain                = (const WGPUChainedStruct*)&shaderModuleWGSLDesc;

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDesc);
    WEBGPU_DEMO_CHECK(shaderModule, "[WebGPU] Failed to create shader module");
    WEBGPU_DEMO_LOG("[WebGPU] Shader module created");

    // === Create the bind group layout ===
    // Bind groups contain binding layouts that describe how uniforms and textures are passed to shaders.

    // Entry for the uniform
    WGPUBindGroupLayoutEntry uniformBindLayout = {};
    uniformBindLayout.binding                  = 0;
    uniformBindLayout.visibility               = WGPUShaderStage_Vertex;
    uniformBindLayout.buffer.type              = WGPUBufferBindingType_Uniform;
    uniformBindLayout.buffer.minBindingSize    = sizeof(ShaderUniformData);

    // Entry for the texture
    WGPUBindGroupLayoutEntry textureBindLayout = {};
    textureBindLayout.binding                  = 1;
    textureBindLayout.visibility               = WGPUShaderStage_Fragment;
    textureBindLayout.texture.sampleType       = WGPUTextureSampleType_Float;
    textureBindLayout.texture.viewDimension    = WGPUTextureViewDimension_2D;

    // Entry for the sampler
    WGPUBindGroupLayoutEntry samplerBindLayout = {};
    samplerBindLayout.binding                  = 2;
    samplerBindLayout.visibility               = WGPUShaderStage_Fragment;
    samplerBindLayout.sampler.type             = WGPUSamplerBindingType_Filtering;

    std::vector<WGPUBindGroupLayoutEntry> bindGroupLayoutEntries = {uniformBindLayout,
                                                                    textureBindLayout,
                                                                    samplerBindLayout};

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.label                         = "Demo Bind Group Layout";
    bindGroupLayoutDesc.entryCount                    = bindGroupLayoutEntries.size();
    bindGroupLayoutDesc.entries                       = bindGroupLayoutEntries.data();
    WGPUBindGroupLayout bindGroupLayout               = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDesc);
    WEBGPU_DEMO_CHECK(bindGroupLayout, "[WebGPU] Failed to create bind group layout");
    WEBGPU_DEMO_LOG("[WebGPU] Bind group layout created");

    // === Create the bind group ===
    // The bind group actually binds the buffer to uniforms and textures.

    WGPUBindGroupEntry uniformBinding = {};
    uniformBinding.binding            = 0;
    uniformBinding.buffer             = uniformBuffer;
    uniformBinding.offset             = 0;
    uniformBinding.size               = sizeof(ShaderUniformData);

    WGPUBindGroupEntry textureBinding = {};
    textureBinding.binding            = 1;
    textureBinding.textureView        = textureView;

    WGPUBindGroupEntry samplerBinding = {};
    samplerBinding.binding            = 2;
    samplerBinding.sampler            = sampler;

    std::vector<WGPUBindGroupEntry> bindGroupEntries = {uniformBinding,
                                                        textureBinding,
                                                        samplerBinding};

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.layout                  = bindGroupLayout;
    bindGroupDesc.entryCount              = bindGroupEntries.size();
    bindGroupDesc.entries                 = bindGroupEntries.data();
    WGPUBindGroup bindGroup               = wgpuDeviceCreateBindGroup(device, &bindGroupDesc);
    WEBGPU_DEMO_CHECK(bindGroup, "[WebGPU] Failed to create bind group");
    WEBGPU_DEMO_LOG("[WebGPU] Bind group created");

    // === Create the pipeline layout ===
    // The pipeline layout specifies the bind groups the pipeline uses.

    WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.label                        = "Demo Pipeline Layout";
    pipelineLayoutDesc.bindGroupLayoutCount         = 1;
    pipelineLayoutDesc.bindGroupLayouts             = &bindGroupLayout;
    WGPUPipelineLayout pipelineLayout               = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDesc);
    WEBGPU_DEMO_CHECK(pipelineLayout, "[WebGPU] Failed to create pipeline layout");
    WEBGPU_DEMO_LOG("[WebGPU] Pipeline layout created");

    // === Create the render pipeline ===
    // The render pipeline specifies the configuration for the fixed-function stages as well as
    // the shaders for the programmable stages of the hardware pipeline.

    // Description of the vertex attributes for the vertex buffer layout
    WGPUVertexAttribute positionAttribute = {};
    positionAttribute.format              = WGPUVertexFormat_Float32x3;
    positionAttribute.offset              = 0;
    positionAttribute.shaderLocation      = 0;

    WGPUVertexAttribute uvsAttribute = {};
    uvsAttribute.format              = WGPUVertexFormat_Float32x2;
    uvsAttribute.offset              = 3 * sizeof(float);
    uvsAttribute.shaderLocation      = 1;

    std::vector<WGPUVertexAttribute> vertexAttributes = {positionAttribute, uvsAttribute};

    // Description of the vertex buffer layout for the vertex shader stage
    WGPUVertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.arrayStride            = 5ull * sizeof(float);
    vertexBufferLayout.stepMode               = WGPUVertexStepMode_Vertex;
    vertexBufferLayout.attributeCount         = vertexAttributes.size();
    vertexBufferLayout.attributes             = vertexAttributes.data();

    // Configuration for the vertex shader stage
    WGPUVertexState vertexState = {};
    vertexState.module          = shaderModule;
    vertexState.entryPoint      = "vs_main";
    vertexState.bufferCount     = 1;
    vertexState.buffers         = &vertexBufferLayout;

    // Configuration for the primitive assembly and rasterization stages
    WGPUPrimitiveState primitiveState = {};
    primitiveState.topology           = WGPUPrimitiveTopology_TriangleList;
    primitiveState.stripIndexFormat   = WGPUIndexFormat_Undefined;
    primitiveState.frontFace          = WGPUFrontFace_CCW;
    primitiveState.cullMode           = WGPUCullMode_None;

    // Configuration for depth testing and stencil buffer
    WGPUDepthStencilState depthStencilState    = {};
    depthStencilState.format                   = depthTextureFormat;
    depthStencilState.depthWriteEnabled        = true;
    depthStencilState.depthCompare             = WGPUCompareFunction_Less;
    depthStencilState.stencilReadMask          = 0;
    depthStencilState.stencilWriteMask         = 0;
    depthStencilState.stencilFront.compare     = WGPUCompareFunction_Always;
    depthStencilState.stencilFront.failOp      = WGPUStencilOperation_Keep;
    depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation_Keep;
    depthStencilState.stencilFront.passOp      = WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.compare      = WGPUCompareFunction_Always;
    depthStencilState.stencilBack.failOp       = WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.depthFailOp  = WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.passOp       = WGPUStencilOperation_Keep;

    // Configuration for multisampling
    WGPUMultisampleState multisampleState = {};
    multisampleState.count                = 1;
    multisampleState.mask                 = WGPUColorWriteMask_All;

    // Configuration for the fragment shader stage
    WGPUColorTargetState colorTargetState = {};
    colorTargetState.format               = surfaceCapabilities.formats[0];
    colorTargetState.writeMask            = WGPUColorWriteMask_All;
    WGPUFragmentState fragmentState       = {};
    fragmentState.module                  = shaderModule;
    fragmentState.entryPoint              = "fs_main";
    fragmentState.targetCount             = 1;
    fragmentState.targets                 = &colorTargetState;

    // Configuration for the entire pipeline
    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.label                        = "Demo Pipeline";
    pipelineDesc.layout                       = pipelineLayout;
    pipelineDesc.vertex                       = vertexState;
    pipelineDesc.primitive                    = primitiveState;
    pipelineDesc.depthStencil                 = &depthStencilState;
    pipelineDesc.multisample                  = multisampleState;
    pipelineDesc.fragment                     = &fragmentState;

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);
    WEBGPU_DEMO_CHECK(pipeline, "[WebGPU] Failed to create render pipeline");
    WEBGPU_DEMO_LOG("[WebGPU] Render pipeline created");

    float rotation = 0.0f;

    // === Render loop ===

    while (!glfwWindowShouldClose(window))
    {
        // === Create a WebGPU texture view ===
        // The texture view is where we render our image into.

        // Get a texture from the surface to render into.
        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

        // The surface might change over time.
        // For example, the window might be resized or minimized.
        // We have to check the status and adapt to it.
        switch (surfaceTexture.status)
        {
            case WGPUSurfaceGetCurrentTextureStatus_Success:
                // Everything is ok.
                // TODO: check for a suboptimal texture and re-configure it if needed.
                break;

            case WGPUSurfaceGetCurrentTextureStatus_Timeout:
            case WGPUSurfaceGetCurrentTextureStatus_Outdated:
            case WGPUSurfaceGetCurrentTextureStatus_Lost:
                // The surface needs to be re-configured.

                // Get the window size from the GLFW window.
                glfwGetWindowSize(window, &surfaceWidth, &surfaceHeight);

                // The surface size might be zero if the window is minimized.
                if (surfaceWidth != 0 && surfaceHeight != 0)
                {
                    WEBGPU_DEMO_LOG("[WebGPU] Re-configuring surface");
                    surfaceConfig.width  = surfaceWidth;
                    surfaceConfig.height = surfaceHeight;
                    wgpuSurfaceConfigure(surface, &surfaceConfig);

                    // Recreate the depth texture.

                    wgpuTextureViewRelease(textureView);
                    wgpuTextureDestroy(depthTexture);
                    wgpuTextureRelease(depthTexture);

                    depthTextureDesc.size.width  = surfaceWidth;
                    depthTextureDesc.size.height = surfaceHeight;

                    depthTexture = wgpuDeviceCreateTexture(device, &depthTextureDesc);
                    WEBGPU_DEMO_CHECK(depthTexture, "[WebGPU] Failed to re-create depth texture");
                    WEBGPU_DEMO_LOG("[WebGPU] Depth texture re-created");

                    depthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDesc);
                    WEBGPU_DEMO_CHECK(depthTextureView, "[WebGPU] Failed to re-create depth texture view");
                    WEBGPU_DEMO_LOG("[WebGPU] Depth texture view re-created");
                }

                // Skip this frame.
                glfwPollEvents();
                continue;

            case WGPUSurfaceGetCurrentTextureStatus_OutOfMemory:
            case WGPUSurfaceGetCurrentTextureStatus_DeviceLost:
                // An error occured.
                WEBGPU_DEMO_CHECK(false, "[WebGPU] Failed to acquire current surface texture");
                break;

            case WGPUSurfaceGetCurrentTextureStatus_Force32:
                break;
        }

        // Create a view into the texture to specify where and how to modify the texture.
        WGPUTextureView view = wgpuTextureCreateView(surfaceTexture.texture, nullptr);

        // === Prepare uniform data ===
        float aspectRatio = static_cast<float>(surfaceWidth) / static_cast<float>(surfaceHeight);

        SLMat4f projectionMatrix;
        projectionMatrix.perspective(70.0f, aspectRatio, 0.1, 1000.0f);

        SLMat4f viewMatrix;
        viewMatrix.translate(0.0f, 0.0f, -2.0f);

        SLMat4f modelMatrix;
        modelMatrix.rotation(90.0f * static_cast<float>(glfwGetTime()), SLVec3f::AXISY);

        // === Update uniforms ===
        ShaderUniformData uniformData = {};
        std::memcpy(uniformData.projectionMatrix, projectionMatrix.m(), sizeof(uniformData.projectionMatrix));
        std::memcpy(uniformData.viewMatrix, viewMatrix.m(), sizeof(uniformData.viewMatrix));
        std::memcpy(uniformData.modelMatrix, modelMatrix.m(), sizeof(uniformData.modelMatrix));
        wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &uniformData, sizeof(ShaderUniformData));

        // === Create a WebGPU command encoder ===
        // The encoder encodes the commands for the GPU into a command buffer.

        WGPUCommandEncoderDescriptor cmdEncoderDesc = {};
        cmdEncoderDesc.label                        = "Demo Command Encoder";

        WGPUCommandEncoder cmdEncoder = wgpuDeviceCreateCommandEncoder(device, &cmdEncoderDesc);
        WEBGPU_DEMO_CHECK(cmdEncoder, "[WebGPU] Failed to create command encoder");

        // === Create a WebGPU render pass ===
        // The render pass specifies what attachments to use while rendering.
        // A color attachment specifies what view to render into and what to do with the texture before and after
        // rendering. We clear the texture before rendering and store the results after rendering.
        // The depth attachment specifies what depth texture to use.

        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.view                          = view;
        colorAttachment.loadOp                        = WGPULoadOp_Clear;
        colorAttachment.storeOp                       = WGPUStoreOp_Store;
        colorAttachment.clearValue.r                  = 0.3;
        colorAttachment.clearValue.g                  = 0.0;
        colorAttachment.clearValue.b                  = 0.2;
        colorAttachment.clearValue.a                  = 1.0;

        WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        depthStencilAttachment.view                                 = depthTextureView;
        depthStencilAttachment.depthLoadOp                          = WGPULoadOp_Clear;
        depthStencilAttachment.depthStoreOp                         = WGPUStoreOp_Store;
        depthStencilAttachment.depthClearValue                      = 1.0f;
        depthStencilAttachment.depthReadOnly                        = false;
        depthStencilAttachment.stencilLoadOp                        = WGPULoadOp_Clear;
        depthStencilAttachment.stencilStoreOp                       = WGPUStoreOp_Store;
        depthStencilAttachment.stencilClearValue                    = 0.0f;
        depthStencilAttachment.stencilReadOnly                      = true;

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.label                    = "Demo Render Pass";
        renderPassDesc.colorAttachmentCount     = 1;
        renderPassDesc.colorAttachments         = &colorAttachment;
        renderPassDesc.depthStencilAttachment   = &depthStencilAttachment;

        // === Encode the commands ===
        // The commands to begin a render pass, bind a pipeline, draw the triangle and end the render pass
        // are encoded into a buffer.

        WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(cmdEncoder, &renderPassDesc);
        wgpuRenderPassEncoderSetPipeline(renderPassEncoder, pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, vertexBuffer, 0, vertexDataSize);
        wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, indexBuffer, WGPUIndexFormat_Uint16, 0, indexDataSize);
        wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, bindGroup, 0, nullptr);
        wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indexCount, 1, 0, 0, 0);
        wgpuRenderPassEncoderEnd(renderPassEncoder);

        // === Get the command buffer ===
        // The command encoder is finished to get the commands for the GPU to execute in a command buffer.

        WGPUCommandBufferDescriptor cmdBufferDesc = {};
        cmdBufferDesc.label                       = "Demo Command Buffer";

        WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(cmdEncoder, &cmdBufferDesc);

        // === Submit the command buffer to the GPU ===
        // The work for the GPU is submitted through the queue and executed.
        wgpuQueueSubmit(queue, 1, &cmdBuffer);

        // === Present the surface ===
        // This presents our rendered texture to the screen.
        wgpuSurfacePresent(surface);

        // === Clean up resources ===
        wgpuCommandBufferRelease(cmdBuffer);
        wgpuRenderPassEncoderRelease(renderPassEncoder);
        wgpuCommandEncoderRelease(cmdEncoder);
        wgpuTextureViewRelease(view);
        wgpuTextureRelease(surfaceTexture.texture);

        glfwPollEvents();
    }

    // === Release all WebGPU resources ===

    wgpuRenderPipelineRelease(pipeline);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuBindGroupRelease(bindGroup);
    wgpuBindGroupLayoutRelease(bindGroupLayout);
    wgpuShaderModuleRelease(shaderModule);
    wgpuSamplerRelease(sampler);
    wgpuTextureViewRelease(textureView);
    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
    wgpuTextureViewRelease(depthTextureView);
    wgpuTextureDestroy(depthTexture);
    wgpuTextureRelease(depthTexture);
    wgpuBufferDestroy(uniformBuffer);
    wgpuBufferRelease(uniformBuffer);
    wgpuBufferDestroy(indexBuffer);
    wgpuBufferRelease(indexBuffer);
    wgpuBufferDestroy(vertexBuffer);
    wgpuBufferRelease(vertexBuffer);
    wgpuSurfaceRelease(surface);
    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuInstanceRelease(instance);
    WEBGPU_DEMO_LOG("[WebGPU] Resources released");

    // === Destroy the window and terminate GLFW ===

    glfwDestroyWindow(window);
    glfwTerminate();
    WEBGPU_DEMO_LOG("[GLFW] Window closed and terminated");

    return 0;
}