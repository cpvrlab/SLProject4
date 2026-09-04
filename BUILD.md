# Building SLProject4

This file is the self-contained path from a fresh clone to a running binary.
The [wiki](https://github.com/cpvrlab/SLProject4/wiki) remains the long-form
reference for IDE-specific setup (CLion, QtCreator, VS Code, Visual Studio).

- **C++17**, CMake **≥ 3.15**
- Supported targets: Windows, Linux, macOS (Intel & arm64), Android, iOS,
  and WebAssembly via Emscripten

## Quick start (Windows, Linux, macOS)

```sh
git clone https://github.com/cpvrlab/SLProject4.git
cd SLProject4
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

The demo binary is written to `build/`. Run `app-demo` and use
_File > Load Demo Scenes_ to browse the scenes.

The first configure downloads two archives automatically: the runtime data
(~600 MB of models, textures and videos) and the prebuilt third-party libraries
for your platform. Expect it to take a while. Both are controlled by
`SL_DOWNLOAD_DATA` and `SL_DOWNLOAD_PREBUILTS` below.

**Windows:** clone into a path shorter than 256 characters, or the build fails
with unhelpful errors.

## Prerequisites

### Linux (Ubuntu/Debian)

```sh
sudo apt-get update
sudo apt-get install -y cmake git build-essential
sudo apt-get install -y libxi-dev libxrandr-dev libx11-dev libxinerama-dev \
                        libxcursor-dev libxxf86vm-dev
sudo apt-get install -y libgl1-mesa-dev libglu-dev
sudo apt-get install -y ffmpeg
```

Package names vary between distributions.

### macOS

Xcode command line tools and [CMake](https://cmake.org/download/) ≥ 3.15. To
generate an Xcode project instead of makefiles:

```sh
cmake -B build_xcode -G Xcode
```

### Windows

Visual Studio with the *Desktop development with C++* workload (Community
works), plus [CMake](https://cmake.org/download/). To generate a solution:

```sh
cmake -B build_win64_vs -G "Visual Studio 17 2022" -A x64
```

Open `build_win64_vs/SLProject.sln`, set **app-demo** as the startup project
and run. `cmake -G` with no argument lists every available generator.

## CMake options

All are `ON` unless noted. Pass them at configure time, e.g.
`cmake -B build -DSL_BUILD_WITH_MEDIAPIPE=OFF`.

| Option | Default | Effect |
|---|---|---|
| `SL_DOWNLOAD_DATA` | ON | Download the runtime data archive |
| `SL_DOWNLOAD_PREBUILTS` | ON | Download prebuilt third-party libraries |
| `SL_BUILD_APPS` | ON | Build the sample applications |
| `SL_BUILD_EXERCISES` | ON | Build the course exercises under `apps/exercises` |
| `SL_BUILD_WAI` | ON | Build the WAI (Where-Am-I) SLAM module |
| `SL_BUILD_WEBGPU_DEMO` | ON | Build the WebGPU demo |
| `SL_BUILD_VULKAN_APPS` | **OFF** | Build the Vulkan sample apps |
| `SL_BUILD_WITH_OPTIX` | **OFF** | Build the NVIDIA OptiX ray tracer |
| `SL_BUILD_WITH_KTX` | ON | Khronos texture library |
| `SL_BUILD_WITH_OPENSSL` | ON | HTTPS support |
| `SL_BUILD_WITH_ASSIMP` | ON | 3D model file loading |
| `SL_BUILD_WITH_MEDIAPIPE` | ON | Google MediaPipe hand/face tracking |

Some are forced off per platform in the top-level `CMakeLists.txt`: iOS
disables MediaPipe; Emscripten disables WAI, OpenSSL and MediaPipe. You do not
need to pass those explicitly.

## Android

Requires Android Studio and a device in developer mode with USB debugging. The
emulator will not work — only `arm64-v8a` prebuilts are provided, so x86
emulator images cannot be built.

1. Open `apps/source/platforms/android/example_project/` in Android Studio.
2. In `Tools > SDK Manager > SDK Tools`, install **NDK 21.4.7075529**,
   **CMake 3.22.1** and the Google USB driver. These exact versions are pinned
   in `app/build.gradle`.
3. Select your device and press Run.

To build a different app, set `Command-line Options` under
`Settings > Build, Execution, Deployment > Gradle-Android Compiler` to
`-PappLibraryName=<app name>`, e.g. `-PappLibraryName=app-Demo-Node`.

## iOS

Requires Xcode, a physical device and an Apple ID with a free developer
account. Simulators will not work — only device prebuilts are provided.

Find your development team ID in *Keychain Access > login > Certificates*:
open your Apple Development certificate and copy the **Organisational Unit**
field. Then:

```sh
cd scripts
./generate_xcode_project_ios.sh <your development team ID>
```

Open `build_ios/SLProject.xcodeproj`, select the `app-demo` scheme and your
connected device, and run. On first launch the device will refuse the app until
you trust the certificate under
`Settings > General > VPN & Device Management`.

## Web (Emscripten)

Install the SDK, pinning the version CI builds against:

```sh
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.60
./emsdk activate 3.1.60
```

Then build and serve:

```sh
. <emsdk>/emsdk_env
cd <SLProject4>
emcmake cmake -B build-emscripten -DSL_DOWNLOAD_DATA=OFF
cmake --build build-emscripten --target app-demo -j

cd build-emscripten
python3 serve-app-demo.py     # then open http://localhost:8080
```

`emcmake` injects the Emscripten toolchain, which plain `cmake` does not know
about. The `serve-<app>.py` scripts are generated at configure time — one per
app target — and set the COOP/COEP headers the threaded Wasm build needs.

Not every demo scene runs in the browser: SLProject uses WebGL 2 (OpenGL ES
3.0), so scenes needing OpenGL 4 features, and the OpenCV modules that cannot
be compiled to WebAssembly, are unavailable. See the
[Emscripten page](https://cpvrlab.github.io/SLProject4/emscripten.html) in the
documentation for the details.

## Troubleshooting

**Missing headers such as `opencv2/core/core.hpp`.** You configured with
`-DSL_DOWNLOAD_PREBUILTS=OFF` on a tree that has no prebuilts yet. Re-run
CMake with it `ON`.

**The build tries to re-download everything.** The data and prebuilt archives
land in `data/` and `externals/prebuilt/`, both git-ignored. Deleting them
forces a fresh download on the next configure.

**Android build fails on architecture.** Only `arm64-v8a` is supported; there
are no prebuilts for other ABIs.

## Generating the documentation

See [docs/README.md](docs/README.md). The published version is at
[cpvrlab.github.io/SLProject4](https://cpvrlab.github.io/SLProject4/).
