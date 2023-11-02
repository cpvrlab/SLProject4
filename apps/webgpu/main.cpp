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
#endif

#include <webgpu.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>
#include <cstdlib>

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

    // Create a non-resizable window as we currently don't recreate surfaces when the window size changes.
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

    // === Acquire a WebGPU device ===
    // A device provides access to a GPU and is created from an adapter.

    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.label                = "Demo Device";
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

#ifdef SYSTEM_WINDOWS
    WGPUSurfaceDescriptorFromWindowsHWND nativeSurfaceDesc = {};
    nativeSurfaceDesc.chain.sType                          = WGPUSType_SurfaceDescriptorFromWindowsHWND;
    nativeSurfaceDesc.hinstance                            = GetModuleHandle(nullptr);
    nativeSurfaceDesc.hwnd                                 = glfwGetWin32Window(window);
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

    // === Create the shader module ===
    // Create a shader module for use in our render pipeline.
    // Shaders are written in a WebGPU-specific language called WGSL (WebGPU shader language).
    // Shader modules can be used in multiple pipeline stages by specifying different entry points.

    const char* shaderSource = R"(
        @vertex
        fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
            if (in_vertex_index == 0u) {
                return vec4f(-0.5, -0.5, 0.0, 1.0);
            } else if (in_vertex_index == 1u) {
                return vec4f(0.5, -0.5, 0.0, 1.0);
            } else {
                return vec4f(0.0, 0.5, 0.0, 1.0);
            }
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
    shaderModuleDesc.label                      = "Hello Triangle Shader";
    shaderModuleDesc.nextInChain                = (const WGPUChainedStruct*)&shaderModuleWGSLDesc;

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDesc);
    WEBGPU_DEMO_CHECK(shaderModule, "[WebGPU] Failed to create shader module");
    WEBGPU_DEMO_LOG("[WebGPU] Shader module created");

    // === Create the WebGPU render pipeline ===
    // The render pipeline specifies the configuration for the fixed-function stages as well as
    // the shaders for the programmable stages of the hardware pipeline.

    // Configuration for the vertex shader stage
    WGPUVertexState vertexState = {};
    vertexState.module          = shaderModule;
    vertexState.entryPoint      = "vs_main";

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
    pipelineDesc.label                        = "Hello Triangle Pipeline";
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
        WEBGPU_DEMO_CHECK(surfaceTexture.status == WGPUSurfaceGetCurrentTextureStatus_Success, "[WebGPU] Failed to acquire current surface texture");

        // Create a view into the texture to specify where and how to modify the texture.
        WGPUTextureView view = wgpuTextureCreateView(surfaceTexture.texture, nullptr);

        // === Create a WebGPU command encoder ===
        // The encoder encodes the commands for the GPU into a command buffer.

        WGPUCommandEncoderDescriptor cmdEncoderDesc = {};
        cmdEncoderDesc.label                        = " Hello Triangle Command Encoder";

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
        renderPassDesc.label                    = "Hello Triangle Render Pass";
        renderPassDesc.colorAttachmentCount     = 1;
        renderPassDesc.colorAttachments         = &colorAttachment;

        // === Encode the commands ===
        // The commands to begin a render pass, bind a pipeline, draw the triangle and end the render pass
        // are encoded into a buffer.

        WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(cmdEncoder, &renderPassDesc);
        wgpuRenderPassEncoderSetPipeline(renderPassEncoder, pipeline);
        wgpuRenderPassEncoderDraw(renderPassEncoder, 3, 1, 0, 0);
        wgpuRenderPassEncoderEnd(renderPassEncoder);

        // === Get the command buffer ===
        // The command encoder is finished to get the commands for the GPU to execute in a command buffer.

        WGPUCommandBufferDescriptor cmdBufferDesc = {};
        cmdBufferDesc.label                       = "Hello Triangle Command Buffer";

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