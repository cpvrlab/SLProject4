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

    WGPURequiredLimits requiredLimits                     = {};
    requiredLimits.limits.maxVertexAttributes             = 1;
    requiredLimits.limits.maxVertexBuffers                = 1;
    requiredLimits.limits.maxBufferSize                   = 4ull * 2ull * sizeof(float);
    requiredLimits.limits.maxVertexBufferArrayStride      = 2ull * sizeof(float);
    requiredLimits.limits.minStorageBufferOffsetAlignment = adapterLimits.limits.minStorageBufferOffsetAlignment;
    requiredLimits.limits.minUniformBufferOffsetAlignment = adapterLimits.limits.minUniformBufferOffsetAlignment;

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
    surfaceConfig.presentMode              = WGPUPresentMode_Fifo;
    surfaceConfig.alphaMode                = surfaceCapabilities.alphaModes[0];
    surfaceConfig.width                    = surfaceWidth;
    surfaceConfig.height                   = surfaceHeight;
    wgpuSurfaceConfigure(surface, &surfaceConfig);
    WEBGPU_DEMO_LOG("[WebGPU] Surface configured");

    // === Create the WebGPU vertex buffer ===
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

    // === Create the WebGPU index buffer ===

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

    // === Create the shader module ===
    // Create a shader module for use in our render pipeline.
    // Shaders are written in a WebGPU-specific language called WGSL (WebGPU shader language).
    // Shader modules can be used in multiple pipeline stages by specifying different entry points.

    const char* shaderSource = R"(
        @vertex
        fn vs_main(@location(0) in_vertex_position: vec2f) -> @builtin(position) vec4f {
            return vec4f(in_vertex_position, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4f {
            return vec4f(0.2, 0.2, 1.0, 1.0);
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

    // === Create the WebGPU render pipeline ===
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
    wgpuShaderModuleRelease(shaderModule);
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