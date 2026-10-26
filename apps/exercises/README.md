# Exercises

The course exercises of the BFH modules Computer Graphics, Image Processing and
Image Analysis moved to the BFH exercises repository:
https://gitlab.ti.bfh.ch/courses/exercises-repo

There they build with their own CMake project, without SLProject4, and are
named after the chapters of the CPVR scripts:

| SLProject4 (removed)               | exercises-repo                                  |
|------------------------------------|-------------------------------------------------|
| ch04_HelloCube_Python              | exercises/cg/py/cg04_HelloCube.py               |
| ch06_ColorCube                     | exercises/cg/cpp/cg06_ColorCube                 |
| ch07_DiffuseCube                   | exercises/cg/cpp/cg07_DiffuseCube               |
| ch07_DiffuseSphere                 | exercises/cg/cpp/cg07_DiffuseSphere             |
| ch08_BlinnPhongLighting            | exercises/cg/cpp/cg08_BlinnPhongLighting        |
| ch09_TextureMapping                | exercises/cg/cpp/cg09_TextureMapping            |
| ch09_TextureMapping_Python         | exercises/cg/py/cg09_TextureMapping.py          |
| cv01_ChangeBrightnessAndContrast   | exercises/cv/cpp/ip05_BrightnessAndContrast     |
| cv02_ClarendonFilter               | exercises/cv/cpp/ip05_ClarendonFilter           |
| cv06_WarpTriangle                  | exercises/cv/cpp/ip08_WarpTriangle              |
| cv07_MeshWarping                   | exercises/cv/cpp/ip08_MeshWarping               |
| cv08_MeshMorphing                  | exercises/cv/cpp/ip08_MeshMorphing              |
| cv13_Snapchat2D                    | exercises/cv/cpp/ip08_Snapchat2D                |
| cv15_YoloV3                        | exercises/cv/cpp/ia07_YoloV3                    |
| cv13_FaceTracking                  | exercises/cv/cpp/ia08_FaceTracking              |
| cv13_FacialLandmarkDetection       | exercises/cv/cpp/ia08_FacialLandmarkDetection   |
| cv13_HeadPoseEstimation            | exercises/cv/cpp/ia08_HeadPoseEstimation        |

The shaders ch06_ColorCube, ch07_DiffuseLighting, ch08_BlinnPhongLighting and
ch09_TextureMapping moved to exercises/data/shaders (with the prefix cg).

The examples that remain here (ch00_new_cpp_features, ch03_Bresenham_Net,
the Java, .NET, Vulkan and WebGL variants, Geom and ParticleGenerator) are not
part of the exercises repository. ch03_Bresenham_Net is replaced by
exercises/cg/py/cg03_Bresenham.py and will be removed here once the CG script
refers to the Python version.
