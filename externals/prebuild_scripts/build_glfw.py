#!/usr/bin/python3

import platform
import subprocess
import os
import multiprocessing
import shutil
from pathlib import Path


BUILD_DIR = Path("build")
INSTALL_DIR = Path("install")

SYSTEM_WIN64 = "win64"
SYSTEM_LINUX = "linux"
SYSTEM_MAC64 = "mac64"
SYSTEM_MACARM64 = "macArm64"


is_windows = platform.system() == "Windows"
is_linux = platform.system() == "Linux"
is_darwin = platform.system() == "Darwin"


def build(build_type):
    build_dir = BUILD_DIR / build_type
    install_dir = INSTALL_DIR / build_type

    # Convert the first letter to uppercase for CMake.
    cmake_build_type = build_type[0].upper() + build_type[1:]

    config_command = [
        "cmake",
        "-B" + str(build_dir),
        "-DCMAKE_INSTALL_PREFIX=" + str(install_dir),
        "-DGLFW_BUILD_EXAMPLES=OFF",
        "-DGLFW_BUILD_TESTS=OFF",
        "-DGLFW_BUILD_DOCS=OFF"
    ]

    if not is_windows:
        config_command.append("-DCMAKE_BUILD_TYPE=" + cmake_build_type)
    
    if system == SYSTEM_MAC64:
        config_command.append("-DCMAKE_OSX_ARCHITECTURES=x86_64")
    elif system == SYSTEM_MACARM64:
        config_command.append("-DCMAKE_OSX_ARCHITECTURES=arm64")

    build_command = [
        "cmake",
        "--build", build_dir,
        "--target", "install",
        "-j" + str(multiprocessing.cpu_count())
    ]

    if is_windows:
        build_command.extend(["--config", cmake_build_type])

    subprocess.run(config_command)
    subprocess.run(build_command)

    destination = output_dir / build_type
    destination.mkdir()
    
    if is_windows:
        shutil.copy(install_dir / "lib" / "glfw3.lib", destination)
    else:
        shutil.copy(install_dir / "lib" / "libglfw3.a", destination)


print(f"\n=== Building GLFW ===\n")
version = input("Version: ")

if is_windows:
    system = SYSTEM_WIN64
    print("System: " + system)
elif is_linux:
    system = SYSTEM_LINUX
    print("System: " + system)
elif is_darwin:
    system = input(f"System ({SYSTEM_MAC64}/{SYSTEM_MACARM64}): ")
    assert system in (SYSTEM_MAC64, SYSTEM_MACARM64)

name = system + "_glfw_" + version

prebuilt_dir = Path(os.curdir).absolute().parent / "prebuilt"
output_dir = Path(prebuilt_dir, name)
print("Output directory: " + str(output_dir))

if not os.path.isdir("glfw"):
    print("\n=== Cloning GLFW ===\n")
    subprocess.run(["git", "clone", "https://github.com/glfw/glfw.git"])

os.chdir("glfw")

print(f"\n=== Checking out version {version} ===\n")
subprocess.run(["git", "checkout", version])
subprocess.run(["git", "pull", "origin", version])

print(f"\n=== Preparing output directory ===\n")

shutil.rmtree(output_dir, ignore_errors=True)
output_dir.mkdir()

print("Directory created")

print(f"\n=== Building debug version ===\n")
build("debug")
print(f"\n=== Building release version ===\n")
build("release")

shutil.copytree(INSTALL_DIR / "debug" / "include", output_dir / "include")
shutil.copy("LICENSE.md", output_dir)
shutil.copy("README.md", output_dir)

print("\n=== Creating archive === \n")

os.chdir(os.pardir)
shutil.make_archive(name, "zip", output_dir.parent, output_dir.name)
print("Archive created")
print("Path: " + str(Path(name + ".zip").absolute()))

print("\n")