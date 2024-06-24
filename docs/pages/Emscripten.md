\page emscripten Emscripten

SLProject can be built to run in a web browser. We use the [Emscripten](https://emscripten.org/) toolchain, which compiles C/C++ code to a binary instruction format called [WebAssembly](https://webassembly.org/). The generated Wasm module can be loaded by ordinary JavaScript at runtime. The browser then compiles the instructions to native machine code and runs it, making it possible to run high-performance code in the browser. To access OpenGL functions, browsers expose the [WebGL API](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API), which Emscripten wraps in standard OpenGL headers. SLProject uses WebGL 2, which is based on OpenGL 3.0 ES. 
Not all scenes from app-Demo-SLProject can run in the browser because OpenGL 4.0 functions are not available or because some OpenCV modules can't be compiled for WebAssembly.

\tableofcontents

<h2>How it works</h2>

The Emscripten toolchain is made from the [Clang compiler](https://clang.llvm.org/), some runtime libraries, an implementation of the C, C++ and POSIX APIs, and ports of some popular libraries such as SDL, GLFW, libpng or zlib. This allows us to take code written for desktop platforms and port it to the Web without much effort.

<div style="width: 100%; justify-content: center; display: flex">
    <img src="images/emscripten_apis.svg" width="40%">
</div>

Emscripten uses standard browser APIs to implement its libraries. For example, a call to the C function ```printf``` probably uses ```console.log``` internally. Here is how a few commonly used libraries are implented behind the scenes:

- Threads: [Web Workers](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API)
- Sockets: [WebSockets](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API)
- GLFW: [Canvas](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)
- OpenGL: [WebGL](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API)
- OpenAL: [Web Audio](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)

<h2>Browser Interaction</h2>

Even though Emscripten provides APIs for most browser features, it is sometimes still necessary to call some JavaScript functions directly. For example, there is currently no camera API. Emscripten provides a ```EM_ASM``` macro to embed JavaScript code directly into C/C++ code, which is comparable to inline assembly in native compilers. This allows us to call browser APIs directly in C++ and to pass values between JavaScript and C++. The JavaScript code can only run on the main thread, so we use the ```MAIN_THREAD_EM_ASM``` macro.

<h2>Threading</h2>

<h3>JavaScript Is Asynchronous</h3>

JavaScript runs its code in a [event loop](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Event_loop). Code is executed when an event
is triggered, such as a keypress or a new animation frame. The event is processed completely and then the browser can continue its work. This
is why you can't run an infinite loop in JavaScript as this would block the browser window. I/O function such as downloading files don't
block, but rather take a callback as argument that the browser calls when the data is available. This asynchronous thinking is very different
from languages such as C++ or Java where you usually block until an operation is complete. This now poses a problem: How do we download files
like textures synchronously so they are available when we want to use them in the next function call?

<h3>Web Workers</h3>

The solution is to run the entire application in a [Web Worker](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API). A
Web Worker runs its code in a separate thread, which is allowed to block. We add the flag ```-sPROXY_TO_PTHREAD``` to the Emscripten linker,
which generates a small main function that runs the original main function in a different thread. Now we are able to synchronously download assets from the server. Before starting the download, we start a spinner animation so the user knows that the app is loading and not crashing.

<h3>OffscreenCanvas</h3>

The canvas element where our graphics are drawn is not accessible in Web Workers. Fortunately, modern browsers implement
an interface called [OffscreenCanvas](https://developer.mozilla.org/en-US/docs/Web/API/OffscreenCanvas). This interface allows us to
transfer the canvas from the main thread to our Web Worker and render into it. We use the Emscripten linker flag ```-sOFFSCREENCANVAS_SUPPORT=1```
to enable this feature and the flag ```-sOFFSCREENCANVASES_TO_PTHREAD='#canvas'``` to transfer the HTML canvas element to our Web Worker.

<h2>File I/O</h2>

When running natively, SLProject uses the file system to load and store data. In the browser environment, the type of storage depends on the asset type.

<ul>
<li>
    <b>Images</b> are downloaded from a remote server (\ref SLIOReaderFetch). To store images, we open a popup in the browser that displays the image and contains a download link (\ref SLIOWriterBrowserPopup).
</li>
<li>
    <b>Models and fonts</b> are downloaded from a remote server (\ref SLIOReaderFetch).
</li>
<li>
    <b>Shader source files</b> are downloaded from a remote server (\ref SLIOReaderFetch).
</li>
<li>
    <b>Generated shaders</b> are stored to and loaded from memory (\ref SLIOWriterMemory and \ref SLIOReaderMemory)
</li>
<li>
    <b>Config files</b> are usually stored to and loaded from [browser local storage](https://developer.mozilla.org/en-US/docs/Web/API/Window/localStorage) (\ref SLIOReaderLocalStorage and SLIOWriterLocalStorage). Static config files (e.g. calibrations) are downloaded from a remote server (\ref SLIOReaderFetch).
</li>
</ul>