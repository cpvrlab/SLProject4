\page emscripten Emscripten
SLProject can be built to run in a web browser. We use the [Emscripten](https://emscripten.org/) toolchain, which compiles C/C++ code to a binary instruction format called [WebAssembly](https://webassembly.org/). The generated Wasm module can be loaded by ordinary JavaScript at runtime. The browser then compiles the instructions to native machine code and runs it, making it possible to run high-performance code in the browser. To access OpenGL functions, browsers expose the [WebGL API](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API), which Emscripten wraps in standard OpenGL headers. SLProject uses WebGL 2, which is based on OpenGL 3.0 ES. 
Not all scenes from app-demo can run in the browser because OpenGL 4.0 functions are not available or because some OpenCV modules can't be compiled for WebAssembly.

\tableofcontents

<h2>How it works</h2>

The Emscripten toolchain contains a modified [Clang compiler](https://clang.llvm.org/), some runtime libraries, an implementation of the C, C++ and POSIX APIs, and ports of some popular libraries such as SDL, GLFW, libpng or zlib. This allows us to take code written for desktop platforms and port it to the Web without much effort.

<div style="width: 100%; justify-content: center; display: flex">
    <img src="images/emscripten_apis.svg" width="40%">
</div>

Emscripten uses standard browser APIs to implement its libraries. For example, a call to the C function `printf` might use the JavaScript function `console.log` internally. Here is how a few commonly used libraries are implented behind the scenes:

- Threads: [Web Workers](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API)
- Sockets: [WebSockets](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API)
- GLFW: [Canvas](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)
- OpenGL: [WebGL](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API)
- OpenAL: [Web Audio](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)

<h2>Browser Interaction</h2>

Even though Emscripten provides APIs for most browser features, it is sometimes still necessary to call some JavaScript functions directly. For example, there is currently no camera API. Emscripten provides a `EM_ASM` macro to embed JavaScript code directly into C/C++ code, which is comparable to inline assembly in native compilers. This allows us to call browser APIs directly in C++ and to pass values between JavaScript and C++.

<h2>JavaScript Is Asynchronous</h2>

JavaScript runs its code in a [event loop](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Event_loop). Code is executed when an event
is triggered, such as a keypress or a new animation frame. The browser can only continue its work when the event is processed by the JavaScript code. This
is why you can't run an infinite loop in JavaScript as this would block the browser window. JavaScript APIs usually don't block for this reason, but
return a [`Promise`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise) that represents an
asynchronous operation. The calling code can then specify callbacks that are run when the operation completes or fails.

This asynchronous thinking leads to some limitations when developing for Emscripten:
  - There is no main `while` loop in `AppEmscripten.cpp`. Instead, we submit a function to the browser
    that is called when a new animation frame is available. The Emscripten function to do this is `emscripten_request_animation_frame`.
  - I/O functions are not allowed on the main thread. I/O operations like downloading files from a server
    cannot run on the main thread because they run synchronously and block their thread. This problem
    is usually handled by submitting a task to the `SLAssetLoader` to download the files on its worker thread instead.
  - Core assets like fonts or common shaders are loaded asynchronously. On all platforms other than Emscripten, assets that
    are used in all scenes are loaded synchronously before starting the main loop. On Emscripten, this task is done on
    the worker thread of the `SLAssetLoader` because I/O is not allowed on the main thread.
  - The number of threads is limited. The Emscripten implementation of C++ threads can only start the backing Web Worker
    once we yield to the browser event loop. This means that after spawning a thread, we cannot use it directly
    because its Web Worker hasn't started yet. The solution is to create a pool of Web Workers before running the `main` function
    that are used when a new thread is created. This is done by adding the linker flag
    `-sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency + 4` that sets the Web Worker pool size to the number of
    logical processors available plus 4 for good measure.
    For more information, see [the Pthreads page in the Emscripten docs](https://emscripten.org/docs/porting/pthreads.html#special-considerations).

<h2>File I/O</h2>

When running natively, SLProject uses the file system to load and store data. In the browser environment, the type of storage depends on the asset type.

- **Images** are downloaded from a remote server (`SLIOReaderFetch`). To store images, we open a popup in the browser that displays the image and contains a download link (`SLIOWriterBrowserPopup`).
- **Models and fonts** are downloaded from a remote server (`SLIOReaderFetch`).
- **Shader source files** are downloaded from a remote server (`SLIOReaderFetch`).
- **Generated shaders** are stored to and loaded from memory (`SLIOWriterMemory` and `SLIOReaderMemory`)
- **Config files** are usually stored to and loaded from [browser local storage](https://developer.mozilla.org/en-US/docs/Web/API/Window/localStorage) (`SLIOReaderLocalStorage` and `SLIOWriterLocalStorage`).
  Static config files (e.g. calibrations) are downloaded from a remote server (`SLIOReaderFetch`).
