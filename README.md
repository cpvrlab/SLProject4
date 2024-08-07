# Welcome to the SLProject4

[![Build Windows](https://github.com/cpvrlab/SLProject4/actions/workflows/build-x86_64-windows.yml/badge.svg)](https://github.com/cpvrlab/SLProject4/actions/workflows/build-x86_64-windows.yml)
[![Build macOS-x86](https://github.com/cpvrlab/SLProject4/actions/workflows/build-x86_64-macos.yml/badge.svg)](https://github.com/cpvrlab/SLProject4/actions/workflows/build-x86_64-macos.yml)
[![Build macOS-arm64](https://github.com/cpvrlab/SLProject4/actions/workflows/build-arm64-macos.yml/badge.svg)](https://github.com/cpvrlab/SLProject4/actions/workflows/build-arm64-macos.yml)
[![Build linux-x86](https://github.com/cpvrlab/SLProject4/actions/workflows/build-x86_64-linux.yml/badge.svg?branch=main)](https://github.com/cpvrlab/SLProject4/actions/workflows/build-x86_64-linux.yml)
[![Build Emscripten](https://github.com/cpvrlab/SLProject4/actions/workflows/build-wasm-emscripten.yml/badge.svg)](https://github.com/cpvrlab/SLProject4/actions/workflows/build-wasm-emscripten.yml)
[![Build & Deploy Docs](https://github.com/cpvrlab/SLProject4/actions/workflows/deploy-pages.yml/badge.svg)](https://github.com/cpvrlab/SLProject4/actions/workflows/deploy-pages.yml)


SL stands for Scene Library. It is developed at the Bern University of Applied Sciences (BFH) in Switzerland and is used for student projects in the cpvrLab. The various applications show what you can learn in three semesters about 3D computer graphics in real-time rendering and ray tracing. The framework is built in C++ and OpenGL ES and can be built for Windows, Linux, macOS (Intel & arm64), Android, Apple iOS, and WebAssembly-enabled browsers. The framework can render alternatively with Ray Tracing and Path Tracing, which provides high-quality transparencies, reflections, and soft shadows. For a complete feature list see the [SLProject4 wiki](https://github.com/cpvrlab/SLProject4/wiki).

## How to get the SLProject4

The SLProject4 is hosted on GitHub as a GIT repository.
[GIT](http://git-scm.com/) is a distributed versioning control system.

To clone SLProject4, use the following command:

```
cd <Path to where you want the SLProject4 folder>
git clone https://github.com/cpvrlab/SLProject4.git
```

For people with an aversion to the command line, a GIT GUI tool, such as the [GitHub Desktop Client](https://desktop.github.com), can be used. To get the latest additions to SLProject4, please check the develop branch:

```
git checkout develop
```

For detailed build instructions see the [SLProject4 wiki](https://github.com/cpvrlab/SLProject4/wiki).
For detailed framework information see the [HTML documentation](https://cpvrlab.github.io/SLProject4/).
