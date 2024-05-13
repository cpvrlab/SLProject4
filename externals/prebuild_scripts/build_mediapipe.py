import subprocess
from pathlib import Path
from urllib.request import urlretrieve
import os
import shutil
import platform


MEDIAPIPE_VERSION = "v0.10.9"
OPENCV_VERSION = "4.7.0"

BAZEL_VERSION = "6.4.0"
MEDIAPIE_REPO_URL = "https://github.com/google/mediapipe.git"
C_API_ROOT = "mediapipe/tasks/c"

BUILD_CONFIGURATIONS = [
    ("debug", "dbg"),
    ("release", "opt"),
]

HEADERS = [
    "components/containers/category.h",
    "components/containers/classification_result.h",
    "components/containers/detection_result.h",
    "components/containers/embedding_result.h",
    "components/containers/keypoint.h",
    "components/containers/landmark.h",
    "components/containers/rect.h",
    "components/processors/classifier_options.h",
    "components/processors/embedder_options.h",
    "core/base_options.h",
    "text/language_detector/language_detector.h",
    "text/text_classifier/text_classifier.h",
    "text/text_embedder/text_embedder.h",
    "vision/core/common.h",
    "vision/gesture_recognizer/gesture_recognizer_result.h",
    "vision/gesture_recognizer/gesture_recognizer.h",
    "vision/hand_landmarker/hand_landmarker_result.h",
    "vision/hand_landmarker/hand_landmarker.h",
    "vision/image_classifier/image_classifier.h",
    "vision/image_embedder/image_embedder.h",
    "vision/object_detector/object_detector.h",
]

OPENCV_BUILD_DIR_NAME = "opencv_build"


def run_command(command, cwd=None):
    print(64 * "-")
    print("> " + " ".join(command))
    result = subprocess.run(command, cwd=cwd)
    print(64 * "-")
    return result


def replace_line(path, line_number, new_content):
    with open(path) as file:
        lines = file.readlines()
    
    lines[line_number] = new_content + "\n"

    with open(path, "w") as file:
        file.writelines(lines)


def replace_section(path, line_number, new_content):
    with open(path) as file:
        lines = file.readlines()
    
    for i, new_line in enumerate(new_content.splitlines()):
        lines[line_number + i] = new_line + "\n"

    with open(path, "w") as file:
        file.writelines(lines)


def append_lines(path, lines):
    with open(path, "a") as file:
        for line in lines:
            file.write(line + "\n")


if __name__ == "__main__":
    env_path = Path("mediapipe_build")
    if not env_path.is_dir():
        env_path.mkdir()
    os.chdir(env_path)

    is_x86_64 = platform.machine().lower() in ("x86_64", "amd64")
    is_aarch64 = platform.machine().lower() in ("aarch64", "arm64")

    is_windows = platform.system() == "Windows"
    is_darwin = platform.system() == "Darwin"

    if is_x86_64:
        arch_name = "x86_64"
    elif is_aarch64:
        arch_name = "aarch64"
    else:
        print(f"Error: Unknown target architecture '{platform.system()}'")
        exit(1)

    if is_windows:
        os_name = "windows"
    elif is_darwin:
        os_name = "darwin"
    else:
        print(f"Error: Unknown target system '{platform.machine()}'")
        exit(1)

    output_dir = Path(f"mediapipe-{arch_name}-{os_name}").absolute()
    print(f"Output directory: {output_dir}")

    repo_path = Path("mediapipe").absolute()

    if not repo_path.exists():
        print("Cloning MediaPipe...")
        run_command(["git", "clone", MEDIAPIE_REPO_URL])

        print(f"Checking out version {MEDIAPIPE_VERSION}...")
        run_command(["git", "-C", str(repo_path), "checkout", MEDIAPIPE_VERSION])

        print("Patching...")

        if is_windows:
            append_lines(repo_path / "mediapipe" / "tasks" / "c" / "vision" / "BUILD", [
                "",
                "cc_binary(",
                "    name = \"vision.dll\",",
                "    linkshared = True,",
                "    tags = [",
                "        \"manual\",",
                "        \"nobuilder\",",
                "        \"notap\",",
                "    ],",
                "    deps = VISION_LIBRARIES,",
                ")"
            ])

            opencv_build_path = repo_path / "third_party/opencv_windows.BUILD"
            opencv_version_suffix = OPENCV_VERSION.replace(".", "")

            replace_line(repo_path / "WORKSPACE", 369, "    path = \"../opencv_install\"")
            replace_line(opencv_build_path, 7, f"OPENCV_VERSION = \"{opencv_version_suffix}\"")
            
            replace_section(opencv_build_path, 25,
"""        ":opt_build": glob([
            "x64/vc17/lib/opencv_*" + OPENCV_VERSION + ".lib",
            "x64/vc17/bin/opencv_*" + OPENCV_VERSION + ".dll",
        ]),
        ":dbg_build": glob([
            "x64/vc17/lib/opencv_*" + OPENCV_VERSION + "d.lib",
            "x64/vc17/bin/opencv_*" + OPENCV_VERSION + "d.dll",
        ]),
    }),
    hdrs = glob(["include/opencv4/opencv2/**/*.h*"]),""")

            replace_section(repo_path / "mediapipe" / "tasks" / "c" / "vision" / "image_embedder" / "image_embedder.cc", 107,
"""              MpImage::IMAGE_FRAME,
              {
                  static_cast<::ImageFormat>(image_frame->Format()),
                  image_frame->PixelData(),
                  image_frame->Width(),
                  image_frame->Height()}};""")
    
            replace_section(repo_path / "mediapipe" / "tasks" / "c" / "vision" / "image_classifier" / "image_classifier.cc", 102,
"""              MpImage::IMAGE_FRAME,
              {
                  static_cast<::ImageFormat>(image_frame->Format()),
                  image_frame->PixelData(),
                  image_frame->Width(),
                  image_frame->Height()}};""")

            replace_section(repo_path / "mediapipe" / "tasks" / "c" / "vision" / "hand_landmarker" / "hand_landmarker.cc", 110,
"""              MpImage::IMAGE_FRAME,
              {
                  static_cast<::ImageFormat>(image_frame->Format()),
                  image_frame->PixelData(),
                  image_frame->Width(),
                  image_frame->Height()}};""")

            replace_section(repo_path / "mediapipe" / "tasks" / "c" / "vision" / "object_detector" / "object_detector.cc", 117,
"""              MpImage::IMAGE_FRAME,
              {
                  static_cast<::ImageFormat>(image_frame->Format()),
                  image_frame->PixelData(),
                  image_frame->Width(),
                  image_frame->Height()}};""")
        elif is_darwin:
            opencv_build_path = repo_path / "third_party/opencv_macos.BUILD"

            replace_line(repo_path / "WORKSPACE", 363, "    path = \"../opencv_install\"")
            replace_line(opencv_build_path, 11, "PREFIX = \"\"")
            replace_line(opencv_build_path, 27, "    hdrs = glob([paths.join(PREFIX, \"include/opencv4/opencv2/**/*.h*\")]),")
            replace_line(opencv_build_path, 28, "    includes = [paths.join(PREFIX, \"include/opencv4\")],")
    else:
        print(f"MediaPipe repository found: {repo_path}")

    bazel_dir = Path("bazel").absolute()
    
    if is_windows:
        bazel_path = bazel_dir / "bazel.exe"
    else:
        bazel_path = bazel_dir / "bazel"

    if not bazel_dir.exists():
        print("Downloading Bazel...")
        
        bazel_dir.mkdir()
        print("  Directory created")

        print("  Downloading...")

        if is_x86_64:
            bazel_arch = "x86_64"
        elif is_aarch64:
            bazel_arch = "arm64"

        if is_windows:
            bazel_system = "windows"
        elif is_darwin:
            bazel_system = "darwin"

        url = f"https://github.com/bazelbuild/bazel/releases/download/{BAZEL_VERSION}/bazel-{BAZEL_VERSION}-{bazel_system}-{bazel_arch}"
        if is_windows:
            url += ".exe"

        urlretrieve(url, bazel_path)
        print("  Bazel downloaded")

        if not is_windows:
            os.chmod(bazel_path, 0o755)
            print("  Bazel made executable")
    else:
        print(f"Bazel found: {bazel_path}")

    venv_dir = Path("venv").absolute()
    
    if is_windows:
        python_path = venv_dir / "Scripts" / "python.exe"
        pip_path = venv_dir / "Scripts" / "pip.exe"
    else:
        python_path = venv_dir / "bin" / "python"
        pip_path = venv_dir / "bin" / "pip"

    if not venv_dir.exists():
        print("Creating venv...")
        
        if is_windows:
            run_command(["python", "-m", "venv", "venv"])
        else:
            run_command(["python3", "-m", "venv", "venv"])

        print("Installing numpy...")
        run_command([str(pip_path), "install", "numpy"])
    else:
        print(f"Python venv found: {venv_dir}")
        print(f"Python path: {python_path}")

    opencv_path = Path("opencv")
    if not opencv_path.is_dir():
        print("Cloning OpenCV....")
        run_command(["git", "clone", "https://github.com/opencv/opencv.git"])

        print(f"Checking out version {OPENCV_VERSION}...")
        run_command(["git", "-C", str(opencv_path), "checkout", OPENCV_VERSION])

    opencv_contrib_path = Path("opencv_contrib")
    if not opencv_contrib_path.is_dir():
        print("Cloning OpenCV's extra modules....")
        run_command(["git", "clone", "https://github.com/opencv/opencv_contrib.git"])

        print(f"Checking out version {OPENCV_VERSION}...")
        run_command(["git", "-C", str(opencv_contrib_path), "checkout", OPENCV_VERSION])

    opencv_install_path = Path("opencv_install")
    if not opencv_install_path.is_dir():
        print("Building OpenCV...")

        if is_windows:
            run_command([
                "cmake",
                f"-B{OPENCV_BUILD_DIR_NAME}",
                "-DBUILD_ZLIB=OFF",
                f"-DOPENCV_EXTRA_MODULES_PATH={opencv_contrib_path}/modules",
                f"-DCMAKE_INSTALL_PREFIX={opencv_install_path}",
                f"{opencv_path}"
            ])

            for config in ("Debug", "Release"):
                run_command([
                    "cmake",
                    "--build", OPENCV_BUILD_DIR_NAME,
                    "--target", "install",
                    "--config", config,
                    "-j8"
                ])
        else:
            for config in ("Debug", "Release"):
                run_command([
                    "cmake",
                    f"-B{OPENCV_BUILD_DIR_NAME}",
                    f"-DCMAKE_BUILD_TYPE={config}",
                    "-DBUILD_ZLIB=OFF",
                    f"-DOPENCV_EXTRA_MODULES_PATH={opencv_contrib_path}/modules",
                    f"-DCMAKE_INSTALL_PREFIX={opencv_install_path}",
                    f"{opencv_path}"
                ])

                run_command([
                    "cmake",
                    "--build", OPENCV_BUILD_DIR_NAME,
                    "--target", "install",
                    "-j8"
                ])


    if output_dir.exists():
        print("Cleaning output directory...")
        shutil.rmtree(output_dir)
    else:
        print("Creating output directory...")
    
    output_dir.mkdir()

    if is_windows:
        bazel_python_path = str(python_path).replace("\\", "/")
        os.environ["BAZEL_SH"] = "C:/Program Files/Git/bin/sh.exe"

        for config_name, bazel_config in BUILD_CONFIGURATIONS:
            print(f"Building {config_name} configuration...")

            result = run_command([
                str(bazel_path),
                "build",
                "-c", bazel_config,
                "--copt", "/DMP_EXPORT=__declspec(dllexport)",
                "--action_env", f"PYTHON_BIN_PATH={bazel_python_path}",
                "--define", "MEDIAPIPE_DISABLE_GPU=1",
                "//mediapipe/tasks/c/vision:vision.dll"
            ], cwd=repo_path)

            if result.returncode == 0:
                print("Build finished!")
            else:
                print("Build failed!")
                exit(1)

            print(f"Copying {config_name} build artifacts...")

            dst_lib_dir = output_dir / config_name / "lib"
            dst_lib_dir.mkdir(parents=True)

            dst_bin_dir = output_dir / config_name / "bin"
            dst_bin_dir.mkdir(parents=True)

            shutil.copyfile("mediapipe/bazel-bin/mediapipe/tasks/c/vision/vision.dll", dst_bin_dir / "vision.dll")
            shutil.copyfile("mediapipe/bazel-bin/mediapipe/tasks/c/vision/vision.dll.if.lib", dst_lib_dir / "vision.lib")
    else:
        for config_name, bazel_config in BUILD_CONFIGURATIONS:
            print(f"Building {config_name} configuration...")
            
            result = run_command([
                str(bazel_path),
                "build",
                "-c", bazel_config,
                "--action_env", f"PYTHON_BIN_PATH={python_path}",
                "--define", "MEDIAPIPE_DISABLE_GPU=1",
                "//mediapipe/tasks/c/vision:libvision.dylib"
            ], cwd=repo_path)

            if result.returncode == 0:
                print("Build finished!")
            else:
                print("Build failed!")
                exit(1)

            print(f"Copying {config_name} build artifacts...")

            dst_lib_dir = output_dir / config_name / "lib"
            dst_lib_dir.mkdir(parents=True)
            shutil.copyfile("mediapipe/bazel-bin/mediapipe/tasks/c/vision/libvision.dylib", dst_lib_dir / "libvision.dylib")

    print("Copying headers...")

    src_headers_dir = repo_path / C_API_ROOT
    dst_headers_dir = output_dir / "include" / C_API_ROOT

    for header in HEADERS:
        src_path = src_headers_dir / header
        dst_path = dst_headers_dir / header

        if not dst_path.parent.exists():
            dst_path.parent.mkdir(parents=True)
        
        shutil.copyfile(src_path, dst_path)
    
    print("Copying license...")
    shutil.copyfile(repo_path / "LICENSE", output_dir / "LICENSE")

    print("Done!")
