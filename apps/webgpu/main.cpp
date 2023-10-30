#include <webgpu.h>

#include <iostream>

int main(int argc, const char* argv[]) {
    WGPUInstance instance = wgpuCreateInstance(nullptr);
    if (instance) {
        std::cout << "[WebGPU] Instance created" << std::endl;
    } else {
        std::cerr << "[WebGPU] Failed to create instance" << std::endl;
        return 1;
    }

    wgpuInstanceRelease(instance);
    std::cerr << "[WebGPU] Resources released" << std::endl;

    return 0;
}