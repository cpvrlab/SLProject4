\page introduction Introduction
\htmlonly
\endhtmlonly
\tableofcontents

\section overview Overview

There are 5 code sections in SLProject:

- **APP**: Application code
  - OS-dependent OpenGL context and window creation
  - UI definition using ImGUI
  - Scene definition using SL
  - Video processing using CV
- **SL**: Scene Library code
  - Scene & scene view management
  - Scenegraph classes
  - 3D and 2D rendering
  - Animation
- **CV**: Computer vision code:
  - Video capturing
  - Calibration
  - Tracking
- **Utils**: Utilities used by SL and CV
- **Externals**: External dependencies

<img src="images/SLProject-App-Structure.png" width="30%">

\section diagram Class Diagram

The following class diagram gives you an overview of the major classes with its important attributes and methods:

- The **brown boxes** on top contain the application code that depend on the OS and do the GUI, the scene assembly and the video processing.
- The **light blue classes** are the central classes with the top-level instances of SLAssetManager and SLInputManager. The core classes for the scene are SLScene and SLSceneView. The UI is redered with SLImGui.
- The **dark blue classes** are the alternative renderers for ray tracing and path tracing.
- The **yellow classes** define the materials that are responsible for the visual appearances of the mesh objects.
- The **green classes** build the scene graph that defines the spacial structure of the visible objects.
- The **pink classes** define a single triangulated mesh object.
- The **violet classes** encapsulate all OpenGL vertex array object and buffer objects.
- The **red classes** build the animation framework.
- The **peach classes** to the right encapsulate the video, image and AR tracking functionality using OpenCV. CV classes are independent from all SL classes. Only SLGLTexture uses CVImage for its texture images.
- The **red classes** build the animation framework.
- The **white classes** are low level classes for the math. Some of them are within the namespace Utils.
- The **gray boxes** at the bottom are the external libraries that are used within the frame work.

<img src="images/SLProject_UML_min.svg" style="min-width: 64vw; position: relative; left: calc(-32vw + 50%)">

\section app Application Code

The application's code (grey boxes at the top of the diagram) contains the code for the operating system, the scene definition with SLProject library (SL), the video processing using CV-classes and the UI with ImGUI. In all cases we have the most outer shell of the application that handles the window and the OpenGL context creation and passes the events to a thin C-function interface before it is handled by the C++-framework in the library lib-SLProject.

The following platforms are supported and applications are provided for demonstration:

- **Windows, Linux and macOS** applications use the [GLFW](http://www.glfw.org/)
  C-library for the platform independent window and context creation.
  GLFW is included in the SLProject repository. See the app-Demo-GLFW for demonstration. 
- **Android** applications start in Java and pass the events using the JNI (Java
  Native Interface) to the C-interface. See the [wiki for build instructions](https://github.com/cpvrlab/SLProject4/wiki/Build-for-Android)
  and `apps/source/platforms/android/example_project` for the project used in demos. 
- **Apple iOS** applications start in Objective-C before it passes the events to the C-interface.
  See the [wiki for build instructions](https://github.com/cpvrlab/SLProject4/wiki/Build-on-MacOS-with-XCode-for-iOS)
  and `apps/source/platforms/ios/example_project` for the project used in demos. 
- **Web** applications use [Emscripten](https://cpvrlab.github.io/SLProject4/emscripten.html) to compile C++ code to [WebAssembly](https://webassembly.org/). This allows applications to be served by a web server
  and run inside the browser.

For all demo apps (desktop and mobile) we use the [Dear ImGui](https://github.com/ocornut/imgui) library for the UI. The UI for `app-demo` is implemented in AppDemoGui. Dear ImGui is also included in the repository.

For more information about SLProject's app framework, see [this page](#app-framework).

\section central Central Classes

The light blue classes form the center of the SLProject framework:
- SLAssetManager holds the expensive resources such as meshes, materials, textures and shader programs in vectors.
- SLInputManager collects all user events from the mouse and keyboard as well as from additional input devices such as a LeapMotion or Kinect sensor.
- SLScene is the top-level class of the framework that represents the scene with
  its properties. The scene content is created in SLScene::onLoad.
  It also holds one or more pointers to SLSceneView instances.
- SLSceneView represents a dynamic real time 3D view onto the scene.
  A scene can be shown in multiple sceneviews as demonstrated in the app-Viewer-Qt application.
  A scene view receives all events (keyboard, mouse etc.) from the GUI via the SLInputManager.

\section node Scenegraph Classes

SLNode is the major building block for the the scenegraph structure (green classes)
and can have 0-N children nodes and 0-N triangle meshes.
A node can be transformed (translated, rotated and scaled) in 3D-space.

- SLLightDirect, SLLightSpot and SLLightRect are from SLNode derived and define lights  that can be placed and directed in space.
- SLNodeLOD implements the level of detail functionality.
- SLCamera that defines the view to the scene. The scene can have multiple cameras but only one can be active for the scene view.

\section mesh Mesh Classes

SLMesh is the base class for triangulated or wire framed meshes (pink classes). A
mesh is rendered with a material defined in SLMaterial.
A mesh has all the vertex attributes such as position, normals, texture coordinates.
The triangles are defined by indexes into the vertex attribute arrays.
A mesh has an instance of SLGLVertexArray that does all the OpenGL drawing.
This vertex array object (VAO) stores all attributes in either a float or half float
vertex attribute buffer (SLGLVertexBuffer) that is generated in the memory of the GPU.
The attribute data on the client side is not deleted because it is used for ray tracing.

- SLRevolver, SLSphere, SLCylinder, SLCone, SLBox, SLPolygon and SLRectangle are all inheritants from SLMesh and represent the according platonic shapes.
- SLAABBox and SLUniformGrid implement the space partitioning. Every SLNode has an axis aligned AABB that is used for fast frustum culling and ray shooting.

\section vao VertexArray Classes

SLGLVertexArray and SLGLVertexBuffer encapsulate all OpenGL buffer stuff and provides the core
drawing functionality with OpenGL.

\section material Material Classes

SLMaterial is the core of the yellow classes that define the appearance of a mesh.
A material can have one or more texture images and is rendered with a specific shader program written
in the OpenGL shading language (GLSL).

- SLGLTexture defines a texture image with filtering parameters.
- SLGLProgram defines a shader program with at least one vertex and one fragment shader program.
- SLGLProgramGenerated implements the automatic GLSL shader generation based on the material parameters, the lights and if shadow mapping is used.
- SLGLShader defines a vertex or fragment shader where the source code is read from a file.
- All OpenGL code is restricted to the classes beginning with SLGL. (SLGLState, SLGLTexture, SLGLShader, SLGLProgram, SLGLVertexArray and SLGLVertexBuffer.)
- The linear algebra math is implemented in the classes SLMat3, SLMat4, SLVec3, SLVec4 and SLQuat4.

\section animation Animation Classes

The red animation classes provide the functionality for simple node animations or skeletal animations.

- SLAnimManager: A single instance of this class is held by the SLScene instance and is
  responsible for updating the enabled animations and to manage their life time.
  It keeps a list of all skeletons and node animations and also holds a list of
  all animation playback controllers. The update of all animations is done before
  the rendering of all SLSceneView instances.
- SLAnimPlayback manages the playback state and the local time of an SLAnimation.
  It manages the way the time advances and how the animation loops. It has all
  functionality to play, pause, stop, enable, speed up and slow down a playback.
  A list of all SLAnimPlayback instances is held by the SLAnimManager.
- SLAnimation is a container for multiple SLAnimTrack that build an animation.
  E.g. a walk animation would consist of all the SLAnimTrack that make a
  SLSkeleton walk. It also knows the length of the animation.
- SLAnimTrack and SLNodeAnimTrack: An animation track is a track that affects a
  single SLNode or an SLJoint of an SLSkeleton by interpolating its transform.
  It holds therefore a list of SLKeyframe. For a smooth motion it can interpolate
  the transform at a given time between two neighbouring SLKeyframe.
- SLKeyframe and SLTransformKeyframe define a transform at a certain time on an
  SLAnimTrack.
- SLSkeleton: A skeleton is used to animate a hierarchical object like a human figure.
  An SLSkeleton keeps track of its bones (SLJoints) in a tree structure and
  points with _root to the root node of the skeleton hierarchy.
  An SLSkeleton is not actively transforming any SLNode in the scene graph.
  It just keeps track of its transformed SLJoint.
  A mesh that is associated with a skeleton transforms all its vertices every
  frame by the joint weights (Jw). Every vertex of a mesh has weights for up to four joints
  by which it can be influenced.
- SLJoint is a specialised SLNode that represents a single joint (or bone) in a skeleton
  The main addition of SLJoint to the base SLNode is the offset matrix which is the
  inverse transformation of the joint's binding pose in mesh space.
  It is used to transform the vertices of a rigged SLMesh to the origin of the joint
  to be able to manipulate them in the joint's local space.

\section imageprocessing Image and Video Processing Classes

The orange classes provide the functionality for video and image processing using the OpenCV framework. 
The SLProject framework can now process the images from attached live video cameras. This works via OpenCV 
on desktop OS as well as on iOS and Android. The live video image is constantly fed into an OpenGL texture 
that can be used as a texture on an objects material or as the scenes background. With the live video in the background you can create augmented reality (AR) applications. Examples can be found in the demo application under Load Scene > Using Video > Track Chessboard or Track AruCo. 

- CVImage: Replaces the deprecated SLImage class and provides all for loading, saving and converting images. Internally it stores the image in a cv::Mat instance.  
-  CVCapture: Holds static images from the OpenCV video capture or from an external (iOS and Android) video capture service. There is an CVCapture::lastFrame and an CVCapture::lastFrameGray with the gray level version of the last capture video frame.
- CVCalibration holds all functionality to calibrate the video camera. A classic chessboard pattern is used for calibration. In the demo application a special scene is provided for the calibration (Preferences > Video > Calibrate Camera).
- CVTracked is the base class for tracking classes. The scene can have multiple trackers. 
- A tracker is associated with a Node. When the object to be tracked is found, it controls the nodes transform. If the associated node is the scenes active camera a classic augmented reality application can be generated.		
- CVTrackedChessboard tracks the same chessboard that is used for the camera calibration.	
- CVTrackedAruco tracks special markers called AruCo markers. These markers are optimal in tracking performance and stability.	
- CVTrackedFeatures tracks any 2D features. Supported are any feature detectors and descriptors that are provided by OpenCV.
  In addition we include the enhanced ORB feature detector developed by [Raul Mur](https://github.com/raulmur/ORB_SLAM2).

Authors: marcus.hudritsch@bfh.ch \
Date: Juli 2024 \
Copyright (c): 2002-2024 Marcus Hudritsch, Kirchrain 18, 2572 Sutz, Switzerland
