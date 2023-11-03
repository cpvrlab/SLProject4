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

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdint>

#define WEBGPU_DEMO_LOG(msg) std::cout << (msg) << std::endl

#define WEBGPU_DEMO_CHECK(condition, errorMsg) \
    if (!(condition)) \
    { \
        std::cerr << (errorMsg) << std::endl; \
        std::exit(1); \
    }

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
    requiredLimits.limits.maxVertexAttributes              = 1u;
    requiredLimits.limits.maxVertexBuffers                 = 1u;
    requiredLimits.limits.maxBufferSize                    = 4ull * 2ull * sizeof(float);
    requiredLimits.limits.maxVertexBufferArrayStride       = 2u * sizeof(float);
    requiredLimits.limits.maxInterStageShaderComponents    = 2u;
    requiredLimits.limits.maxBindGroups                    = 2u;
    requiredLimits.limits.maxBindingsPerBindGroup          = 2u;
    requiredLimits.limits.maxUniformBuffersPerShaderStage  = 1u;
    requiredLimits.limits.maxUniformBufferBindingSize      = 16ull * 4ull;
    requiredLimits.limits.maxSampledTexturesPerShaderStage = 1u;
    requiredLimits.limits.maxTextureDimension1D            = 2048;
    requiredLimits.limits.maxTextureDimension2D            = 2048;
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
      -0.5,  0.5, // top-left corner
      -0.5, -0.5, // bottom-left corner
       0.5,  0.5, // top-right corner
       0.5, -0.5, // bottom-right corner
    };
    // clang-format on

    unsigned vertexCount    = vertexData.size() / 2;
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

    // clang-format off
    std::vector<std::uint16_t> indexData =
    {
        0, 1, 2,
        2, 1, 3,
    };
    // clang-format on

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
    uniformBufferDesc.size                 = sizeof(float);
    uniformBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;

    WGPUBuffer uniformBuffer = wgpuDeviceCreateBuffer(device, &uniformBufferDesc);
    WEBGPU_DEMO_CHECK(indexBuffer, "[WebGPU] Failed to create uniform buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Uniform buffer created");

    float uniformBufferData = 0.0f;
    wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &uniformBufferData, sizeof(float));

    // === Create the texture ===

    cv::Mat image = cv::imread(std::string(SL_PROJECT_ROOT) + "/data/images/textures/brickwall0512_C.jpg");
    cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);

    unsigned imageWidth = image.cols;
    unsigned imageHeight = image.rows;
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

    // === Create the shader module ===
    // Create a shader module for use in our render pipeline.
    // Shaders are written in a WebGPU-specific language called WGSL (WebGPU shader language).
    // Shader modules can be used in multiple pipeline stages by specifying different entry points.

    const char* shaderSource = R"(
        @group(0) @binding(0) var<uniform> u_time: f32;
        @group(0) @binding(1) var texture: texture_2d<f32>;

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) texture_coords: vec2f,
        }

        @vertex
        fn vs_main(@location(0) in_vertex_position: vec2f) -> VertexOutput {
            var offset = vec2(0.5 * sin(u_time), 0.5 * cos(u_time));
            
            var out: VertexOutput;
            out.position = vec4f(in_vertex_position + offset, 0.0, 1.0);
            out.texture_coords = in_vertex_position + vec2(0.5, 0.5);
            return out;
        }

        @fragment
        fn fs_main(in: VertexOutput) -> @location(0) vec4f {
            return textureLoad(texture, vec2i(i32(512.0 * in.texture_coords.x), i32(512.0 * in.texture_coords.y)), 0).rgba;
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
    WGPUBindGroupLayoutEntry uniformLayout = {};
    uniformLayout.binding                  = 0;
    uniformLayout.visibility               = WGPUShaderStage_Vertex;
    uniformLayout.buffer.type              = WGPUBufferBindingType_Uniform;
    uniformLayout.buffer.minBindingSize    = sizeof(float);

    // Entry for the texture
    WGPUBindGroupLayoutEntry textureLayout = {};
    textureLayout.binding                  = 1;
    textureLayout.visibility               = WGPUShaderStage_Fragment;
    textureLayout.texture.sampleType       = WGPUTextureSampleType_Float;
    textureLayout.texture.viewDimension    = WGPUTextureViewDimension_2D;

    std::vector<WGPUBindGroupLayoutEntry> bindGroupLayoutEntries = {uniformLayout, textureLayout};

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
    uniformBinding.binding             = 0;
    uniformBinding.buffer              = uniformBuffer;
    uniformBinding.offset              = 0;
    uniformBinding.size                = sizeof(float);

    WGPUBindGroupEntry textureBinding = {};
    textureBinding.binding             = 1;
    textureBinding.textureView         = textureView;

    std::vector<WGPUBindGroupEntry> bindGroupEntries = {uniformBinding, textureBinding};

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

    // Description of the vertex attribute for the vertex buffer layout
    WGPUVertexAttribute vertexAttribute = {};
    vertexAttribute.format              = WGPUVertexFormat_Float32x2;
    vertexAttribute.offset              = 0;
    vertexAttribute.shaderLocation      = 0;

    // Description of the vertex buffer layout for the vertex shader stage
    WGPUVertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.arrayStride            = 2ull * sizeof(float);
    vertexBufferLayout.stepMode               = WGPUVertexStepMode_Vertex;
    vertexBufferLayout.attributeCount         = 1;
    vertexBufferLayout.attributes             = &vertexAttribute;

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
    pipelineDesc.multisample                  = multisampleState;
    pipelineDesc.fragment                     = &fragmentState;

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);
    WEBGPU_DEMO_CHECK(pipeline, "[WebGPU] Failed to create render pipeline");
    WEBGPU_DEMO_LOG("[WebGPU] Render pipeline created");

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

        // === Update the uniform ===
        float time = static_cast<float>(glfwGetTime());
        wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &time, sizeof(float));

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

        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.view                          = view;
        colorAttachment.loadOp                        = WGPULoadOp_Clear;
        colorAttachment.storeOp                       = WGPUStoreOp_Store;
        colorAttachment.clearValue.r                  = 0.3;
        colorAttachment.clearValue.g                  = 0.0;
        colorAttachment.clearValue.b                  = 0.2;
        colorAttachment.clearValue.a                  = 1.0;

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.label                    = "Demo Render Pass";
        renderPassDesc.colorAttachmentCount     = 1;
        renderPassDesc.colorAttachments         = &colorAttachment;

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
    wgpuTextureViewRelease(textureView);
    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
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