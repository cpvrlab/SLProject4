#if defined(_WIN32)
#    define SYSTEM_WINDOWS
#    define GLFW_EXPOSE_NATIVE_WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#elif defined(__linux__)
#    define SYSTEM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11
#elif defined(__APPLE__)
#    ifndef SYSTEM_DARWIN
#        define SYSTEM_DARWIN
#    endif
#    define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <webgpu.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <SLMat4.h>
#include <SLVec4.h>

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

/*
To interact with WebGPU, we create objects with a call to wgpu*Create*. This function normally takes the parent
object as a parameter and a WGPU*Descriptor that contains the specification for the object. The returned object
is released after usage with a call to wgpuRelease*. An object can have a label that is used when reporting errors.

WebGPU concepts:
    - WGPUInstance
        The core interface through which all other objects are created.
    
    - WGPUAdapter
        Represents a physical device and is used to query its capabilities and limits.
    
    - WGPUDevice
        Represents a logical device we interact with. It is created by specifying the capabilities we require
        and fails if the adapter does not support them. We cannot create more resources than specified in the
        limits at creation. This is done to prevent a function from working on one device but not on another.
    
    - WGPUQueue
        The queue is where we submit the commands for the GPU to. Everything that is executed on the GPU goes
        through the queue. Examples are executing rendering or compute pipelines, writing to or reading from buffers.
    
    - WGPUSurface
        The surface we draw onto. How it is acquired depends on the OS so we have to manually convert window handles
        from the different platforms to surfaces. The surface has to be reconfigured every time the window size
        changes.
    
    - WGPUBuffer
        Represents a chunk of memory on the GPU. They can be used for example as vertex buffers, uniforms buffers or
        storage buffers. Buffers are created with a fixed size and usage flags that specify e.g. whether we can copy
        to this buffer or whether it is used as an index buffer. The memory for the buffer is automatically allocated
        at creation but has to be deallocated manually using wgpuBufferDestroy. wgpuQueueWriteBuffer is used to upload
        buffer data to the GPU.
    
    - WGPUTexture
        Represents pixel data in the memory of the GPU. It is similar to a buffer in that memory for it is allocated at
        creation, that it has usage flags and that the memory is deallocated using wgpuTextureDestroy. Data is uploaded
        using wgpuQueueWriteTexture. A texture additionally has a size, a format, a mip level count and a sample count.
        Textures can be used in shaders or as a render attachment (e.g. the depth buffer). Mip maps have to be created
        manually.
    
    - WGPUTextureView
        To use a texture, a texture view has to be created. It specifies which part of the texture is accessed
        (which base array layer, how many array layers, which base mip level, which format, ...).
    
    - WGPUTextureSampler
        To read a texture in a shader, a sampler is commonly used. Textures can also be accessed directly without a
        sampler by specifying texel coordinates, which is more like reading the data from a buffer. To get access to
        features like filtering, mipmapping or clamping, a sampler is used.
    
    - WGPURenderPipeline
        Represents a configuration of the GPU pipeline. It specifies completely how input data is transformed into
        output pixels by settings the configuration for the GPU pipeline stages.

        The WebGPU render pipeline model looks like this:
            1. vertex fetch
            2. vertex shader
            3. primitive assembly
            4. rasterization
            5. fragment shader
            6. stencil test and write
            7. depth test and write
            8. blending
            9. write to attachments

        The descriptor for this structure contains the following configurations:

            - layout: WGPUPipelineLayout
            - vertex: WGPUVertexState
                Configuration for the vertex fetch and vertex shader stages.
                Specifies the shader module to run as well as the buffers and constants used.
                A vertex buffer is specified using a WGPUVertexBufferLayout structure that contains a list of
                attributes (WGPUVertexAttribute) along with the stride and step mode (per vertex or per instance).
                Attributes are specified through a format, an offset in the data and a location in the shader.
                The location is a number hard-coded for the attribute in the shader code. 
            - primitive: WGPUPrimitiveState
                Configuration for the primitive assembly and rasterization stages.
                Specifies the topology (triangles, triangle strips, lines, ...), what index format
                is used, how the face orientation of triangles is defined and the cull mode.
            - depthStencil: WGPUDepthStencilState
                Configuration for the depth test and stencil test stages.
                Specifies the format of the depth/stencil buffer and how depth and stencil testing are performed. 
            - multisample: WGPUMultisampleState
                Configuration for multisampling.
                Specifies the number of samples per pixel as well as additional parameters for muiltisampling.
            - fragment: WGPUFragmentState
                Configuration for the fragment shader and blending stages.
                Specifies the shader module to run as well as the buffers and constants used.
                This state also specifies a list of color targets that are rendered into which contains
                additional configuration such as the color attachement format and the blending mode.

    - WGPUShaderModule
        Represents a shader on the GPU. It is created by specifying code in either the WebGPU shading language (WGSL)
        or the SPIR-V format (not support on the Web). This code is then compiled by the WebGPU implementation to a
        backend-specific shader format such as SPIR-V for Vulkan or MSL for Metal. A shader can have multiple entry
        points, making it possible to use one shader module for both the vertex shader and fragment shader stage.

    - WGPUBindGroup
        A list of resources bound in a shader. Resources can be buffers, samplers or texture views. Each bind group
        entry contains a binding (a unique number assigned to the resource in the shader code), the buffer, sampler or
        texture view bound, an offset and a size. Multiple bind groups can be set per render pass.

    - WGPUBindGroupLayout
        A list of layouts for bind groups. A bind group references its layout and they both have to have the same
        number of entries. The entries describe the binding, which shader stages can access it as well as additional
        info depending on the type. For example, buffer bind group layout entries specify whether they are a uniform
        buffer, a storage buffer or read-only storage. 

    - WGPUPipelineLayout
        Specifies the bind groups used in the pipeline.
    
    - WGPUCommandEncoder
        Work for the GPU has to be recorded into a command buffer. This is done using a command encoder. We call
        functions on the encoder (wgpuCommandEncoder*) to encode the commands into a buffer that can be accessed by
        calling wgpuCommandEncoderFinish. 

    - WGPUCommandBuffer
        When all the GPU commands are recorded, wgpuCommandEncoderFinish is called on the queue which returns a
        command buffer. This buffer can then be submitted to the GPU using wgpuQueueSubmit.

    - WGPURenderPassEncoder
        Specifies how a render pipeline is executed. It encodes the pipeline used along with the required vertex
        buffers, index buffers, bind groups, drawing commands, etc. Accessing these render commands is done using a
        specialized object called a render pass encoder. It is created from a command encoder using
        wgpuCommandEncoderBeginRenderPass. 

*/

struct App
{
    GLFWwindow* window        = nullptr;
    int         surfaceWidth  = 0;
    int         surfaceHeight = 0;

    WGPUInstance        instance         = nullptr;
    WGPUAdapter         adapter          = nullptr;
    WGPUDevice          device           = nullptr;
    WGPUQueue           queue            = nullptr;
    WGPUSurface         surface          = nullptr;
    WGPUBuffer          vertexBuffer     = nullptr;
    WGPUBuffer          indexBuffer      = nullptr;
    WGPUBuffer          uniformBuffer    = nullptr;
    WGPUTexture         depthTexture     = nullptr;
    WGPUTextureView     depthTextureView = nullptr;
    WGPUTexture         texture          = nullptr;
    WGPUTextureView     textureView      = nullptr;
    WGPUSampler         sampler          = nullptr;
    WGPUShaderModule    shaderModule     = nullptr;
    WGPUBindGroupLayout bindGroupLayout  = nullptr;
    WGPUBindGroup       bindGroup        = nullptr;
    WGPUPipelineLayout  pipelineLayout   = nullptr;
    WGPURenderPipeline  pipeline         = nullptr;

    WGPUSurfaceConfiguration  surfaceConfig;
    WGPUTextureFormat         depthTextureFormat;
    WGPUTextureDescriptor     depthTextureDesc;
    WGPUTextureViewDescriptor depthTextureViewDesc;

    unsigned vertexDataSize;
    unsigned indexCount;
    unsigned indexDataSize;

    float camRotX = 0.0f;
    float camRotY = 0.0f;
    float camZ    = 2.0f;
};

struct VertexData
{
    float positionX;
    float positionY;
    float positionZ;
    float normalX;
    float normalY;
    float normalZ;
    float uvX;
    float uvY;
};

struct alignas(16) ShaderUniformData
{
    float projectionMatrix[16];
    float viewMatrix[16];
    float modelMatrix[16];
};

static_assert(sizeof(ShaderUniformData) % 16 == 0, "uniform data size must be a multiple of 16");

#ifdef SYSTEM_DARWIN
extern "C" void* createMetalLayer(void* window);
#endif

void reconfigureSurface(App& app)
{
    // Get the window size from the GLFW window.
    glfwGetWindowSize(app.window, &app.surfaceWidth, &app.surfaceHeight);

    // The surface size might be zero if the window is minimized.
    if (app.surfaceWidth == 0 || app.surfaceHeight == 0)
        return;

    WEBGPU_DEMO_LOG("[WebGPU] Re-configuring surface");
    app.surfaceConfig.width  = app.surfaceWidth;
    app.surfaceConfig.height = app.surfaceHeight;
    wgpuSurfaceConfigure(app.surface, &app.surfaceConfig);

    // Recreate the depth texture.

    wgpuTextureViewRelease(app.depthTextureView);
    wgpuTextureDestroy(app.depthTexture);
    wgpuTextureRelease(app.depthTexture);

    app.depthTextureDesc.size.width  = app.surfaceWidth;
    app.depthTextureDesc.size.height = app.surfaceHeight;

    app.depthTexture = wgpuDeviceCreateTexture(app.device, &app.depthTextureDesc);
    WEBGPU_DEMO_CHECK(app.depthTexture, "[WebGPU] Failed to re-create depth texture");
    WEBGPU_DEMO_LOG("[WebGPU] Depth texture re-created");

    app.depthTextureView = wgpuTextureCreateView(app.depthTexture, &app.depthTextureViewDesc);
    WEBGPU_DEMO_CHECK(app.depthTextureView, "[WebGPU] Failed to re-create depth texture view");
    WEBGPU_DEMO_LOG("[WebGPU] Depth texture view re-created");
}

void onPaint(App& app)
{
    if (app.surfaceWidth == 0 || app.surfaceHeight == 0)
        return;

    // Get a texture from the surface to render into.
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(app.surface, &surfaceTexture);

    // The surface might change over time.
    // For example, the window might be resized or minimized.
    // We have to check the status and adapt to it.
    switch (surfaceTexture.status)
    {
        case WGPUSurfaceGetCurrentTextureStatus_Success:
            // Everything is ok, but still check for a suboptimal texture and re-configure it if needed.
            if (surfaceTexture.suboptimal)
            {
                WEBGPU_DEMO_LOG("[WebGPU] Re-configuring currently suboptimal surface");
                reconfigureSurface(app);
                wgpuSurfaceGetCurrentTexture(app.surface, &surfaceTexture);
            }

            break;

        case WGPUSurfaceGetCurrentTextureStatus_Timeout:
        case WGPUSurfaceGetCurrentTextureStatus_Outdated:
        case WGPUSurfaceGetCurrentTextureStatus_Lost:
            // The surface needs to be re-configured.
            reconfigureSurface(app);
            wgpuSurfaceGetCurrentTexture(app.surface, &surfaceTexture);
            break;

        case WGPUSurfaceGetCurrentTextureStatus_OutOfMemory:
        case WGPUSurfaceGetCurrentTextureStatus_DeviceLost:
            // An error occured.
            WEBGPU_DEMO_CHECK(false, "[WebGPU] Failed to acquire current surface texture");
            break;

        case WGPUSurfaceGetCurrentTextureStatus_Force32:
            break;
    }

    // === Create a WebGPU texture view ===
    // The texture view is where we render our image into.

    // Create a view into the texture to specify where and how to modify the texture.
    WGPUTextureView view = wgpuTextureCreateView(surfaceTexture.texture, nullptr);

    // === Prepare uniform data ===
    float aspectRatio = static_cast<float>(app.surfaceWidth) / static_cast<float>(app.surfaceHeight);

    SLMat4f modelMatrix;

    SLMat4f projectionMatrix;
    projectionMatrix.perspective(70.0f, aspectRatio, 0.1, 1000.0f);

    SLMat4f viewMatrix;
    viewMatrix.rotate(app.camRotY, SLVec3f::AXISY);
    viewMatrix.rotate(app.camRotX, SLVec3f::AXISX);
    viewMatrix.translate(0.0f, 0.0f, 2.0f);
    viewMatrix.invert();

    // === Update uniforms ===
    ShaderUniformData uniformData = {};
    std::memcpy(uniformData.projectionMatrix, projectionMatrix.m(), sizeof(uniformData.projectionMatrix));
    std::memcpy(uniformData.viewMatrix, viewMatrix.m(), sizeof(uniformData.viewMatrix));
    std::memcpy(uniformData.modelMatrix, modelMatrix.m(), sizeof(uniformData.modelMatrix));
    wgpuQueueWriteBuffer(app.queue, app.uniformBuffer, 0, &uniformData, sizeof(ShaderUniformData));

    // === Create a WebGPU command encoder ===
    // The encoder encodes the commands for the GPU into a command buffer.

    WGPUCommandEncoderDescriptor cmdEncoderDesc = {};
    cmdEncoderDesc.label                        = "Demo Command Encoder";

    WGPUCommandEncoder cmdEncoder = wgpuDeviceCreateCommandEncoder(app.device, &cmdEncoderDesc);
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
    depthStencilAttachment.view                                 = app.depthTextureView;
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
    wgpuRenderPassEncoderSetPipeline(renderPassEncoder, app.pipeline);
    wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, app.vertexBuffer, 0, app.vertexDataSize);
    wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, app.indexBuffer, WGPUIndexFormat_Uint16, 0, app.indexDataSize);
    wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, app.bindGroup, 0, nullptr);
    wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, app.indexCount, 1, 0, 0, 0);
    wgpuRenderPassEncoderEnd(renderPassEncoder);

    // === Get the command buffer ===
    // The command encoder is finished to get the commands for the GPU to execute in a command buffer.

    WGPUCommandBufferDescriptor cmdBufferDesc = {};
    cmdBufferDesc.label                       = "Demo Command Buffer";

    WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(cmdEncoder, &cmdBufferDesc);

    // === Submit the command buffer to the GPU ===
    // The work for the GPU is submitted through the queue and executed.
    wgpuQueueSubmit(app.queue, 1, &cmdBuffer);

    // === Present the surface ===
    // This presents our rendered texture to the screen.
    wgpuSurfacePresent(app.surface);

    // === Clean up resources ===
    wgpuCommandBufferRelease(cmdBuffer);
    wgpuRenderPassEncoderRelease(renderPassEncoder);
    wgpuCommandEncoderRelease(cmdEncoder);
    wgpuTextureViewRelease(view);
    wgpuTextureRelease(surfaceTexture.texture);
}

void onResize(GLFWwindow* window, int width, int height)
{
    App& app = *((App*)glfwGetWindowUserPointer(window));
    reconfigureSurface(app);
    onPaint(app);
}

void initGLFW(App& app)
{
    // === Initialize GLFW ===

    WEBGPU_DEMO_CHECK(glfwInit(), "[GLFW] Failed to initialize");
    WEBGPU_DEMO_LOG("[GLFW] Initialized");

    // === Create the GLFW window ===

    // Prevent GLFW from creating an OpenGL context as the underlying graphics API probably won't be OpenGL.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    app.window = glfwCreateWindow(1280, 720, "WebGPU Demo", nullptr, nullptr);
    WEBGPU_DEMO_CHECK(app.window, "[GLFW] Failed to create window");
    WEBGPU_DEMO_LOG("[GLFW] Window created");

    glfwSetWindowUserPointer(app.window, &app);
    glfwSetFramebufferSizeCallback(app.window, onResize);
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

void initWebGPU(App& app)
{
    // === Create a WebGPU instance ===
    // The instance is the root interface to WebGPU through which we create all other WebGPU resources.

    app.instance = wgpuCreateInstance(nullptr);
    WEBGPU_DEMO_CHECK(app.instance, "[WebGPU] Failed to create instance");
    WEBGPU_DEMO_LOG("[WebGPU] Instance created");

    // === Acquire a WebGPU adapter ===
    // An adapter provides information about the capabilities of the GPU.

    WGPURequestAdapterOptions adapterOptions = {};

    wgpuInstanceRequestAdapter(app.instance, &adapterOptions, handleAdapterRequest, &app.adapter);

    WGPUSupportedLimits adapterLimits;
    wgpuAdapterGetLimits(app.adapter, &adapterLimits);

    // === Acquire a WebGPU device ===
    // A device provides access to a GPU and is created from an adapter.
    // We specify the capabilities that we require our device to have in requiredLimits.
    // We cannot access more resources than specified in the required limits,
    // which is how WebGPU prevents code from working on one machine and not on another.

    WGPURequiredLimits requiredLimits                      = {};
    requiredLimits.limits.maxVertexAttributes              = 3u;
    requiredLimits.limits.maxVertexBuffers                 = 1u;
    requiredLimits.limits.maxBufferSize                    = 2048ull;
    requiredLimits.limits.maxVertexBufferArrayStride       = sizeof(VertexData);
    requiredLimits.limits.maxInterStageShaderComponents    = 5u;
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

    wgpuAdapterRequestDevice(app.adapter, &deviceDesc, handleDeviceRequest, &app.device);

    // === Acquire a WebGPU queue ===
    // The queue is where the commands for the GPU are submitted to.

    app.queue = wgpuDeviceGetQueue(app.device);
    WEBGPU_DEMO_CHECK(app.queue, "[WebGPU] Failed to acquire queue");
    WEBGPU_DEMO_LOG("[WebGPU] Queue acquired");

    // === Create a WebGPU surface ===
    // The surface is where our rendered images will be presented to.
    // It is created from window handles on most platforms and from a canvas in the browser.

#if defined(SYSTEM_WINDOWS)
    WGPUSurfaceDescriptorFromWindowsHWND nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                          = WGPUSType_SurfaceDescriptorFromWindowsHWND;
    nativeSurfaceDesc.hinstance                            = GetModuleHandle(nullptr);
    nativeSurfaceDesc.hwnd                                 = glfwGetWin32Window(app.window);
#elif defined(SYSTEM_LINUX)
    WGPUSurfaceDescriptorFromXlibWindow nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                         = WGPUSType_SurfaceDescriptorFromXlibWindow;
    nativeSurfaceDesc.display                             = glfwGetX11Display();
    nativeSurfaceDesc.window                              = glfwGetX11Window(app.window);
#elif defined(SYSTEM_DARWIN)
    WGPUSurfaceDescriptorFromMetalLayer nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                         = WGPUSType_SurfaceDescriptorFromMetalLayer;
    nativeSurfaceDesc.layer                               = createMetalLayer(glfwGetCocoaWindow(app.window));
#endif

    WGPUSurfaceDescriptor surfaceDesc = {};
    surfaceDesc.label                 = "Demo Surface";
    surfaceDesc.nextInChain           = (const WGPUChainedStruct*)&nativeSurfaceDesc;

    app.surface = wgpuInstanceCreateSurface(app.instance, &surfaceDesc);
    WEBGPU_DEMO_CHECK(app.surface, "[WebGPU] Failed to create surface");
    WEBGPU_DEMO_LOG("[WebGPU] Surface created");

    // === Configure the surface ===
    // The surface needs to be configured before images can be presented.

    // Query the surface capabilities from the adapter.
    WGPUSurfaceCapabilities surfaceCapabilities;
    wgpuSurfaceGetCapabilities(app.surface, app.adapter, &surfaceCapabilities);

    // Get the window size from the GLFW window.
    glfwGetFramebufferSize(app.window, &app.surfaceWidth, &app.surfaceHeight);

    app.surfaceConfig             = {};
    app.surfaceConfig.device      = app.device;
    app.surfaceConfig.usage       = WGPUTextureUsage_RenderAttachment;
    app.surfaceConfig.format      = surfaceCapabilities.formats[0];
    app.surfaceConfig.presentMode = WGPUPresentMode_Fifo;
    app.surfaceConfig.alphaMode   = surfaceCapabilities.alphaModes[0];
    app.surfaceConfig.width       = app.surfaceWidth;
    app.surfaceConfig.height      = app.surfaceHeight;
    wgpuSurfaceConfigure(app.surface, &app.surfaceConfig);
    WEBGPU_DEMO_LOG("[WebGPU] Surface configured");

    // === Create the vertex buffer ===
    // The vertex buffer contains the input data for the shader.

    // clang-format off
    std::vector<VertexData> vertexData =
    {
        // left
        {-0.5,  0.5, -0.5, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {-0.5, -0.5, -0.5, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        {-0.5,  0.5,  0.5, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        {-0.5, -0.5,  0.5, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f},

        // right
        { 0.5,  0.5,  0.5,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        { 0.5, -0.5,  0.5,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
        { 0.5,  0.5, -0.5,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
        { 0.5, -0.5, -0.5,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f},

        // bottom
        {-0.5, -0.5,  0.5, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        {-0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f},
        { 0.5, -0.5,  0.5, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 0.5, -0.5, -0.5, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f},

        // top
        {-0.5,  0.5, -0.5, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f},
        {-0.5,  0.5,  0.5, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f},
        { 0.5,  0.5, -0.5, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f},
        { 0.5,  0.5,  0.5, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f},

        // back
        { 0.5,  0.5, -0.5, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f},
        { 0.5, -0.5, -0.5, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f},
        {-0.5,  0.5, -0.5, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f},
        {-0.5, -0.5, -0.5, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f},

        // front
        {-0.5,  0.5,  0.5, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f},
        {-0.5, -0.5,  0.5, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f},
        { 0.5,  0.5,  0.5, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f},
        { 0.5, -0.5,  0.5, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f},
    };
    // clang-format on

    unsigned vertexCount = vertexData.size();
    app.vertexDataSize   = vertexData.size() * sizeof(VertexData);

    WGPUBufferDescriptor vertexBufferDesc = {};
    vertexBufferDesc.label                = "Demo Vertex Buffer";
    vertexBufferDesc.size                 = app.vertexDataSize;
    vertexBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;

    app.vertexBuffer = wgpuDeviceCreateBuffer(app.device, &vertexBufferDesc);
    WEBGPU_DEMO_CHECK(app.vertexBuffer, "[WebGPU] Failed to create vertex buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Vertex buffer created");

    // Upload the data to the GPU.
    wgpuQueueWriteBuffer(app.queue, app.vertexBuffer, 0, vertexData.data(), app.vertexDataSize);

    // === Create the index buffer ===

    // clang-format off
    std::vector<std::uint16_t> indexData =
    {
         0,  1,  2,  2,  1,  3, // left
         4,  5,  6,  6,  5,  7, // right
         8,  9, 10, 10,  9, 11, // bottom
        12, 13, 14, 14, 13, 15, // top
        16, 17, 18, 18, 17, 19, // back
        20, 21, 22, 22, 21, 23, // front
    };
    // clang-format on

    app.indexCount    = indexData.size();
    app.indexDataSize = indexData.size() * sizeof(std::uint16_t);

    WGPUBufferDescriptor indexBufferDesc = {};
    indexBufferDesc.label                = "Demo Index Buffer";
    indexBufferDesc.size                 = app.indexDataSize;
    indexBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;

    app.indexBuffer = wgpuDeviceCreateBuffer(app.device, &indexBufferDesc);
    WEBGPU_DEMO_CHECK(app.indexBuffer, "[WebGPU] Failed to create index buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Index buffer created");

    wgpuQueueWriteBuffer(app.queue, app.indexBuffer, 0, indexData.data(), app.indexDataSize);

    // === Create the uniform buffer ===

    WGPUBufferDescriptor uniformBufferDesc = {};
    uniformBufferDesc.label                = "Demo Uniform Buffer";
    uniformBufferDesc.size                 = sizeof(ShaderUniformData);
    uniformBufferDesc.usage                = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;

    app.uniformBuffer = wgpuDeviceCreateBuffer(app.device, &uniformBufferDesc);
    WEBGPU_DEMO_CHECK(app.indexBuffer, "[WebGPU] Failed to create uniform buffer");
    WEBGPU_DEMO_LOG("[WebGPU] Uniform buffer created");

    ShaderUniformData uniformData = {};
    wgpuQueueWriteBuffer(app.queue, app.uniformBuffer, 0, &uniformData, sizeof(ShaderUniformData));

    // === Create the depth texture ===

    app.depthTextureFormat = WGPUTextureFormat_Depth24Plus;

    app.depthTextureDesc                         = {};
    app.depthTextureDesc.dimension               = WGPUTextureDimension_2D;
    app.depthTextureDesc.format                  = app.depthTextureFormat;
    app.depthTextureDesc.mipLevelCount           = 1;
    app.depthTextureDesc.sampleCount             = 1;
    app.depthTextureDesc.size.width              = app.surfaceWidth;
    app.depthTextureDesc.size.height             = app.surfaceHeight;
    app.depthTextureDesc.size.depthOrArrayLayers = 1;
    app.depthTextureDesc.usage                   = WGPUTextureUsage_RenderAttachment;
    app.depthTextureDesc.viewFormatCount         = 1;
    app.depthTextureDesc.viewFormats             = &app.depthTextureFormat;

    app.depthTexture = wgpuDeviceCreateTexture(app.device, &app.depthTextureDesc);
    WEBGPU_DEMO_CHECK(app.depthTexture, "[WebGPU] Failed to create depth texture");
    WEBGPU_DEMO_LOG("[WebGPU] Depth texture created");

    app.depthTextureViewDesc                 = {};
    app.depthTextureViewDesc.aspect          = WGPUTextureAspect_DepthOnly;
    app.depthTextureViewDesc.baseArrayLayer  = 0;
    app.depthTextureViewDesc.arrayLayerCount = 1;
    app.depthTextureViewDesc.baseMipLevel    = 0;
    app.depthTextureViewDesc.mipLevelCount   = 1;
    app.depthTextureViewDesc.dimension       = WGPUTextureViewDimension_2D;
    app.depthTextureViewDesc.format          = app.depthTextureFormat;

    app.depthTextureView = wgpuTextureCreateView(app.depthTexture, &app.depthTextureViewDesc);
    WEBGPU_DEMO_CHECK(app.depthTextureView, "[WebGPU] Failed to create depth texture view");
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
    textureDesc.mipLevelCount           = 4;
    textureDesc.sampleCount             = 1;

    app.texture = wgpuDeviceCreateTexture(app.device, &textureDesc);
    WEBGPU_DEMO_CHECK(app.texture, "[WebGPU] Failed to create texture");
    WEBGPU_DEMO_LOG("[WebGPU] Texture created");

    // Where do we copy the pixel data to?
    WGPUImageCopyTexture destination = {};
    destination.texture              = app.texture;
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

    // Generate mip levels.

    WGPUExtent3D mipLevelSize;
    mipLevelSize.width              = textureDesc.size.width;
    mipLevelSize.height             = textureDesc.size.height;
    mipLevelSize.depthOrArrayLayers = 1;

    for (unsigned mipLevel = 0; mipLevel < textureDesc.mipLevelCount; mipLevel++)
    {
        cv::Mat  mipLevelImage;
        cv::Size cvSize(static_cast<int>(mipLevelSize.width), static_cast<int>(mipLevelSize.height));
        cv::resize(image, mipLevelImage, cvSize);

        std::size_t mipLevelBytes = 4ull * mipLevelSize.width * mipLevelSize.height;

        destination.mipLevel         = mipLevel;
        pixelDataLayout.bytesPerRow  = 4 * mipLevelSize.width;
        pixelDataLayout.rowsPerImage = mipLevelSize.height;

        // Upload the data to the GPU.
        wgpuQueueWriteTexture(app.queue, &destination, mipLevelImage.data, mipLevelBytes, &pixelDataLayout, &mipLevelSize);

        // Scale the image down for the next mip level.
        mipLevelSize.width /= 2;
        mipLevelSize.height /= 2;
    }

    // === Create a texture view into the texture ===
    WGPUTextureViewDescriptor textureViewDesc = {};
    textureViewDesc.aspect                    = WGPUTextureAspect_All;
    textureViewDesc.baseArrayLayer            = 0;
    textureViewDesc.arrayLayerCount           = 1;
    textureViewDesc.baseMipLevel              = 0;
    textureViewDesc.mipLevelCount             = textureDesc.mipLevelCount;
    textureViewDesc.dimension                 = WGPUTextureViewDimension_2D;
    textureViewDesc.format                    = textureDesc.format;

    app.textureView = wgpuTextureCreateView(app.texture, &textureViewDesc);
    WEBGPU_DEMO_CHECK(app.textureView, "[WebGPU] Failed to create texture view");
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
    samplerDesc.lodMaxClamp           = static_cast<float>(textureDesc.mipLevelCount);
    samplerDesc.compare               = WGPUCompareFunction_Undefined;
    samplerDesc.maxAnisotropy         = 1.0f;

    app.sampler = wgpuDeviceCreateSampler(app.device, &samplerDesc);
    WEBGPU_DEMO_CHECK(app.sampler, "[WebGPU] Failed to create sampler");
    WEBGPU_DEMO_LOG("[WebGPU] Sampler created");

    // === Create the shader module ===
    // Create a shader module for use in our render pipeline.
    // Shaders are written in a WebGPU-specific language called WGSL (WebGPU shader language).
    // Shader modules can be used in multiple pipeline stages by specifying different entry points.

    const char* shaderSource = R"(
        struct VertexInput {
            @location(0) position: vec3f,
            @location(1) normal: vec3f,
            @location(2) uv: vec2f,
        };

        struct Uniforms {
            projectionMatrix: mat4x4f,
            viewMatrix: mat4x4f,
            modelMatrix: mat4x4f,
        };

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) worldNormal: vec3f,
            @location(1) uv: vec2f,
        }
        
        const LIGHT_DIR = vec3f(4.0, -8.0, 1.0);

        @group(0) @binding(0) var<uniform> uniforms: Uniforms;
        @group(0) @binding(1) var texture: texture_2d<f32>;
        @group(0) @binding(2) var textureSampler: sampler;

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var localPos = vec4f(in.position, 1.0);
            var worldPos = uniforms.modelMatrix * localPos;

            var out: VertexOutput;
            out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPos;
            out.worldNormal = in.normal;
            out.uv = in.uv;
            return out;
        }

        @fragment
        fn fs_main(in: VertexOutput) -> @location(0) vec4f {
            var baseColor = textureSample(texture, textureSampler, in.uv);
            
            var diffuse = dot(-normalize(in.worldNormal), normalize(LIGHT_DIR));
            diffuse = diffuse * 0.5 + 0.5;

            return vec4(baseColor.rgb * diffuse, baseColor.a);
        }
    )";

    WGPUShaderModuleWGSLDescriptor shaderModuleWGSLDesc = {};
    shaderModuleWGSLDesc.chain.sType                    = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderModuleWGSLDesc.code                           = shaderSource;

    WGPUShaderModuleDescriptor shaderModuleDesc = {};
    shaderModuleDesc.label                      = "Demo Shader";
    shaderModuleDesc.nextInChain                = (const WGPUChainedStruct*)&shaderModuleWGSLDesc;

    app.shaderModule = wgpuDeviceCreateShaderModule(app.device, &shaderModuleDesc);
    WEBGPU_DEMO_CHECK(app.shaderModule, "[WebGPU] Failed to create shader module");
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

    app.bindGroupLayout = wgpuDeviceCreateBindGroupLayout(app.device, &bindGroupLayoutDesc);
    WEBGPU_DEMO_CHECK(app.bindGroupLayout, "[WebGPU] Failed to create bind group layout");
    WEBGPU_DEMO_LOG("[WebGPU] Bind group layout created");

    // === Create the bind group ===
    // The bind group actually binds the buffer to uniforms and textures.

    WGPUBindGroupEntry uniformBinding = {};
    uniformBinding.binding            = 0;
    uniformBinding.buffer             = app.uniformBuffer;
    uniformBinding.offset             = 0;
    uniformBinding.size               = sizeof(ShaderUniformData);

    WGPUBindGroupEntry textureBinding = {};
    textureBinding.binding            = 1;
    textureBinding.textureView        = app.textureView;

    WGPUBindGroupEntry samplerBinding = {};
    samplerBinding.binding            = 2;
    samplerBinding.sampler            = app.sampler;

    std::vector<WGPUBindGroupEntry> bindGroupEntries = {uniformBinding,
                                                        textureBinding,
                                                        samplerBinding};

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.layout                  = app.bindGroupLayout;
    bindGroupDesc.entryCount              = bindGroupEntries.size();
    bindGroupDesc.entries                 = bindGroupEntries.data();
    app.bindGroup                         = wgpuDeviceCreateBindGroup(app.device, &bindGroupDesc);
    WEBGPU_DEMO_CHECK(app.bindGroup, "[WebGPU] Failed to create bind group");
    WEBGPU_DEMO_LOG("[WebGPU] Bind group created");

    // === Create the pipeline layout ===
    // The pipeline layout specifies the bind groups the pipeline uses.

    WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.label                        = "Demo Pipeline Layout";
    pipelineLayoutDesc.bindGroupLayoutCount         = 1;
    pipelineLayoutDesc.bindGroupLayouts             = &app.bindGroupLayout;

    app.pipelineLayout = wgpuDeviceCreatePipelineLayout(app.device, &pipelineLayoutDesc);
    WEBGPU_DEMO_CHECK(app.pipelineLayout, "[WebGPU] Failed to create pipeline layout");
    WEBGPU_DEMO_LOG("[WebGPU] Pipeline layout created");

    // === Create the render pipeline ===
    // The render pipeline specifies the configuration for the fixed-function stages as well as
    // the shaders for the programmable stages of the hardware pipeline.

    // Description of the vertex attributes for the vertex buffer layout
    WGPUVertexAttribute positionAttribute = {};
    positionAttribute.format              = WGPUVertexFormat_Float32x3;
    positionAttribute.offset              = 0;
    positionAttribute.shaderLocation      = 0;

    WGPUVertexAttribute normalAttribute = {};
    normalAttribute.format              = WGPUVertexFormat_Float32x3;
    normalAttribute.offset              = 3 * sizeof(float);
    normalAttribute.shaderLocation      = 1;

    WGPUVertexAttribute uvAttribute = {};
    uvAttribute.format              = WGPUVertexFormat_Float32x2;
    uvAttribute.offset              = 6 * sizeof(float);
    uvAttribute.shaderLocation      = 2;

    std::vector<WGPUVertexAttribute> vertexAttributes = {positionAttribute, normalAttribute, uvAttribute};

    // Description of the vertex buffer layout for the vertex shader stage
    WGPUVertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.arrayStride            = sizeof(VertexData);
    vertexBufferLayout.stepMode               = WGPUVertexStepMode_Vertex;
    vertexBufferLayout.attributeCount         = vertexAttributes.size();
    vertexBufferLayout.attributes             = vertexAttributes.data();

    // Configuration for the vertex shader stage
    WGPUVertexState vertexState = {};
    vertexState.module          = app.shaderModule;
    vertexState.entryPoint      = "vs_main";
    vertexState.bufferCount     = 1;
    vertexState.buffers         = &vertexBufferLayout;

    // Configuration for the primitive assembly and rasterization stages
    WGPUPrimitiveState primitiveState = {};
    primitiveState.topology           = WGPUPrimitiveTopology_TriangleList;
    primitiveState.stripIndexFormat   = WGPUIndexFormat_Undefined;
    primitiveState.frontFace          = WGPUFrontFace_CCW;
    primitiveState.cullMode           = WGPUCullMode_Back;

    // Configuration for depth testing and stencil buffer
    WGPUDepthStencilState depthStencilState    = {};
    depthStencilState.format                   = app.depthTextureFormat;
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
    fragmentState.module                  = app.shaderModule;
    fragmentState.entryPoint              = "fs_main";
    fragmentState.targetCount             = 1;
    fragmentState.targets                 = &colorTargetState;

    // Configuration for the entire pipeline
    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.label                        = "Demo Pipeline";
    pipelineDesc.layout                       = app.pipelineLayout;
    pipelineDesc.vertex                       = vertexState;
    pipelineDesc.primitive                    = primitiveState;
    pipelineDesc.depthStencil                 = &depthStencilState;
    pipelineDesc.multisample                  = multisampleState;
    pipelineDesc.fragment                     = &fragmentState;

    app.pipeline = wgpuDeviceCreateRenderPipeline(app.device, &pipelineDesc);
    WEBGPU_DEMO_CHECK(app.pipeline, "[WebGPU] Failed to create render pipeline");
    WEBGPU_DEMO_LOG("[WebGPU] Render pipeline created");
}

void deinitWebGPU(App& app)
{
    // === Release all WebGPU resources ===

    wgpuRenderPipelineRelease(app.pipeline);
    wgpuPipelineLayoutRelease(app.pipelineLayout);
    wgpuBindGroupRelease(app.bindGroup);
    wgpuBindGroupLayoutRelease(app.bindGroupLayout);
    wgpuShaderModuleRelease(app.shaderModule);
    wgpuSamplerRelease(app.sampler);
    wgpuTextureViewRelease(app.textureView);
    wgpuTextureDestroy(app.texture);
    wgpuTextureRelease(app.texture);
    wgpuTextureViewRelease(app.depthTextureView);
    wgpuTextureDestroy(app.depthTexture);
    wgpuTextureRelease(app.depthTexture);
    wgpuBufferDestroy(app.uniformBuffer);
    wgpuBufferRelease(app.uniformBuffer);
    wgpuBufferDestroy(app.indexBuffer);
    wgpuBufferRelease(app.indexBuffer);
    wgpuBufferDestroy(app.vertexBuffer);
    wgpuBufferRelease(app.vertexBuffer);
    wgpuSurfaceRelease(app.surface);
    wgpuQueueRelease(app.queue);
    wgpuDeviceRelease(app.device);
    wgpuAdapterRelease(app.adapter);
    wgpuInstanceRelease(app.instance);

    WEBGPU_DEMO_LOG("[WebGPU] Resources released");
}

void deinitGLFW(App& app)
{
    // === Destroy the window and terminate GLFW ===

    glfwDestroyWindow(app.window);
    glfwTerminate();
    WEBGPU_DEMO_LOG("[GLFW] Window closed and terminated");
}

int main(int argc, const char* argv[])
{
    App app;
    initGLFW(app);
    initWebGPU(app);

    double lastCursorX = 0.0f;
    double lastCursorY = 0.0f;

    // === Render loop ===

    while (!glfwWindowShouldClose(app.window))
    {
        // === Update cube model matrix ===

        double cursorX;
        double cursorY;
        glfwGetCursorPos(app.window, &cursorX, &cursorY);

        if (glfwGetMouseButton(app.window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
        {
            app.camRotX -= 0.25f * static_cast<float>(cursorY - lastCursorY);
            app.camRotY -= 0.25f * static_cast<float>(cursorX - lastCursorX);
        }

        lastCursorX = cursorX;
        lastCursorY = cursorY;

        onPaint(app);
        glfwPollEvents();
    }

    deinitWebGPU(app);
    deinitGLFW(app);

    return 0;
}