# WebGPU Demo Application

## Overview

WebGPU is a graphics API standard developed by the World Wide Web Consortium (W3C). It is an abstraction layer
over modern graphics APIs like Vulkan, D3D12 and Metal and thus uses modern concepts such as queues, command buffers
or pipelines. WebGPU is supposed to be 'safe' API and is generally simpler to use than Vulkan. Both graphics and
compute functionality is available.

The primary target for WebGPU was the Web as a replacement for the old WebGL API. This means that
the specification is written for a JavaScript API. However, Google and Mozilla have decided to provide their in-browser
implementations as native libraries, so we can use WebGPU in native apps written in C, C++ or Rust. The implementers
have agreed on a common interface for their libraries in form of a header called `webgpu.h`
(https://github.com/webgpu-native/webgpu-headers/).

There are currently three implementations of this header:
- wgpu-native: Mozilla's implementation for Firefox, written in Rust
- Dawn: Google's implementation for Chromium, written in C++
- Emscripten: Translates `webgpu.h` calls to calls to the WebGPU JavaScript API in the browser

WebGPU uses its own shader language called WGSL (WebGPU Shader Language). This is the only shader language supported
in the browsers even though the native implementations also support SPIR-V.

To make porting to WebGPU easier, the two browser vendors provide tools for translating other shader languages to WGSL:
- Naga: Mozilla's shader translator, can be used only as an executable
- Tint: Google's shader translator, can be used as both a C++ library and an executable

This demo uses the wgpu-native implementation because Mozilla releases pre-built binaries that can easily be
downloaded from CMake. Since wgpu-native uses the same interface header as Dawn, it would also be possible to
link Dawn instead of wgpu-native, but we would have to build and distribute it ourselves.

## Usage

WebGPU follows a stateless design as opposed to OpenGL, where much state has to be set globally before making a
draw call. To use WebGPU, we create objects with a call to wgpu*Create*. This function generally takes the parent
object as a parameter and a WGPU*Descriptor that contains the specification for the object. The returned object
is released after usage with a call to wgpuRelease*. An object can have a label that is used when reporting errors.
We can now call functions on the object to interact with them.

### WebGPU concepts

- **WGPUInstance** \
    The core interface through which all other objects are created.

- **WGPUAdapter** \
    Represents a physical device and is used to query its capabilities and limits.

- **WGPUDevice** \
    Represents a logical device we interact with. It is created by specifying the capabilities we require
    and fails if the adapter does not support them. We cannot create more resources than specified in the
    limits at creation. This is done to prevent a function from working on one device but not on another.

- **WGPUQueue** \
    The queue is where we submit the commands for the GPU to. Everything that is executed on the GPU goes
    through the queue. Examples are executing rendering or compute pipelines, writing to or reading from buffers.

- **WGPUSurface** \
    The surface we draw onto. How it is acquired depends on the OS so we have to manually convert window handles
    from the different platforms to surfaces. The surface has to be reconfigured every time the window size
    changes.

- **WGPUBuffer** \
    Represents a chunk of memory on the GPU. They can be used for example as vertex buffers, uniforms buffers or
    storage buffers. Buffers are created with a fixed size and usage flags that specify e.g. whether we can copy
    to this buffer or whether it is used as an index buffer. The memory for the buffer is automatically allocated
    at creation but has to be deallocated manually using wgpuBufferDestroy. wgpuQueueWriteBuffer is used to upload
    buffer data to the GPU.

- **WGPUTexture** \
    Represents pixel data in the memory of the GPU. It is similar to a buffer in that memory for it is allocated at
    creation, that it has usage flags and that the memory is deallocated using wgpuTextureDestroy. Data is uploaded
    using wgpuQueueWriteTexture. A texture additionally has a size, a format, a mip level count and a sample count.
    Textures can be used in shaders or as a render attachment (e.g. the depth buffer). Mip maps have to be created
    manually.

- **WGPUTextureView** \
    To use a texture, a texture view has to be created. It specifies which part of the texture is accessed
    (which base array layer, how many array layers, which base mip level, which format, ...).

- **WGPUTextureSampler** \
    To read a texture in a shader, a sampler is commonly used. Textures can also be accessed directly without a
    sampler by specifying texel coordinates, which is more like reading the data from a buffer. To get access to
    features like filtering, mipmapping or clamping, a sampler is used.

- **WGPURenderPipeline** \
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

    The descriptor for this structure contains the following configuration structures:

    - layout: WGPUPipelineLayout
    - vertex: WGPUVertexState \
        Configuration for the vertex fetch and vertex shader stages.
        Specifies the shader module to run as well as the buffers and constants used.
        A vertex buffer is specified using a WGPUVertexBufferLayout structure that contains a list of
        attributes (WGPUVertexAttribute) along with the stride and step mode (per vertex or per instance).
        Attributes are specified through a format, an offset in the data and a location in the shader.
        The location is a number hard-coded for the attribute in the shader code.
    - primitive: WGPUPrimitiveState \
        Configuration for the primitive assembly and rasterization stages.
        Specifies the topology (triangles, triangle strips, lines, ...), what index format
        is used, how the face orientation of triangles is defined and the cull mode.
    - depthStencil: GPUDepthStencilState \
        Configuration for the depth test and stencil test stages.
        Specifies the format of the depth/stencil buffer and how depth and stencil testing are performed.
    - multisample: WGPUMultisampleState \
        Configuration for multisampling.
        Specifies the number of samples per pixel as well as additional parameters for muiltisampling.
    - fragment: WGPUFragmentState \
        Configuration for the fragment shader and blending stages.
        Specifies the shader module to run as well as the buffers and constants used.
        This state also specifies a list of color targets that are rendered into which contains
        additional configuration such as the color attachement format and the blending mode.

- **WGPUShaderModule** \
    Represents a shader on the GPU. It is created by specifying code in either the WebGPU shading language (WGSL)
    or the SPIR-V format (not support on the Web). This code is then compiled by the WebGPU implementation to a
    backend-specific shader format such as SPIR-V for Vulkan or MSL for Metal. A shader can have multiple entry
    points, making it possible to use one shader module for both the vertex shader and fragment shader stage.

- **WGPUBindGroup** \
    A list of resources bound in a shader. Resources can be buffers, samplers or texture views. Each bind group
    entry contains a binding (a unique number assigned to the resource in the shader code), the buffer, sampler or
    texture view bound, an offset and a size. Multiple bind groups can be set per render pass.

- **WGPUBindGroupLayout** \
    A list of layouts for bind groups. A bind group references its layout and they both have to have the same
    number of entries. The entries describe the binding, which shader stages can access it as well as additional
    info depending on the type. For example, buffer bind group layout entries specify whether they are a uniform
    buffer, a storage buffer or read-only storage.

- **WGPUPipelineLayout** \
    Specifies the bind groups used in the pipeline.

- **WGPUCommandEncoder** \
    Work for the GPU has to be recorded into a command buffer. This is done using a command encoder. We call
    functions on the encoder (wgpuCommandEncoder*) to encode the commands into a buffer that can be accessed by
    calling wgpuCommandEncoderFinish.

- **WGPUCommandBuffer** \
    When all the GPU commands are recorded, wgpuCommandEncoderFinish is called on the queue which returns a
    command buffer. This buffer can then be submitted to the GPU using wgpuQueueSubmit.

- **WGPURenderPassEncoder** \
    Specifies how a render pipeline is executed. It encodes the pipeline used along with the required vertex
    buffers, index buffers, bind groups, drawing commands, etc. Accessing these render commands is done using a
    specialized object called a render pass encoder. It is created from a command encoder using
    wgpuCommandEncoderBeginRenderPass.

## WebGPU vs. Vulkan

Here's a list of things I've noticed are handled differently from Vulkan (as of 2023-11-26):

- There is no multithreading \
    Unlike Vukan, WebGPU is currently single-threaded and doesn't allow encoding command buffers on
    multiple threads.
    Status: https://gpuweb.github.io/gpuweb/explainer/#multithreading

- There is only one queue \
    Vulkan allows you to create multiple queues from different families. For example, you can create a
    graphics and a compute queue and submit commands to them that are processed in parallel on some GPUs.
    In WebGPU, there is only one queue that is acquired using wgpuDeviceGetQueue.
    Discussion: https://github.com/gpuweb/gpuweb/issues/1065

- There is no manual memory allocation \
    In WebGPU, buffers take care of memory allocation internally. Awesome!

- There is no swap chain \
    The swap chain configuration is part of the surface configuration.

- Error handling is very different \
    In Vulkan, there is by default no validation and programs just silently crash and burn. It is possible
    to enable validation layers that print errors with references to the spec. The reason for this is that
    Vulkan wants to avoid validation overhead in release builds. In WebGPU, validation seems mostly up to the
    implementations. We currently use the wgpu-native implementation, which seems to catch all errors and prints
    most of the time a nice error message with a human-readable error message including the labels of the
    problematic objects, suggests fixes and even generates a stack trace. I'm not sure what the overhead of
    this validation is, but I excpect there to be an option to turn it off in the future.