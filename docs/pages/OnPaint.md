\page on-paint How one Frame gets painted

As an example we start here at the outer shell of a desktop app using GLFW.
The most outer onPaint routine gets called in the main event loop
whenever a function that is called within returns true. Repaints occurs therefore as
fast as possible and only when needed. If nothing happens (no animation or
camera move) no repaint is requested and energy can be saved. There is no
timer involved.

* **AppDemoMainGLFW.cpp: onPaint()**:
   * **AppDemoVideo.cpp: onUpdateVideo()**
      * Updates calibration during calibration process.
      * Updates tracking if a tracker is used.
      * Updates the video texture with the latest video frame if video is used.
   * **SLInterface.h: slUpdateScene()**
      * SLScene::onUpdate:
         * 1) Calculates the frame time over all views.
         * 2) SLInputManager::pollAndProcessEvents
         * 3) SLAnimManager::update: Updates all animations.
         * 4) SLNode::updateAABBRec: Updates the axis aligned bounding boxes of all nodes that have changed during animation.
   * **SLInterface.h: slPaintAllViews()**:
      * **SLSceneView::onPaint** called for every sceneview:
         * **SLSceneView::draw3DGL**
            * 1) Render all shadow maps from the light position and direction.
            * 2) Do camera update (smooth transitions from keyboard entries).
            * 3) Clear color and depth buffers.
            * 4) Set Viewports with SLCamera::setViewport depending on its projection
            * 5) Render Background with SLCamera::background: single color, smooth gradient or video image.
            * 6) Set SLCamera::setProjection and SLCamera::setView.
            * 7) Frustum Culling
               * SLCamera::setFrustumPlanes set the cameras frustum planes according to the
                 current projection and view transform.
               * SLNode::cull3DRec gets called on the root node:
                  * All nodes that are checked as visible get assigned to their meshes material nodesVisible3D vector.
            * 8) Skybox Drawing: If a skybox is set, this gets drawn first around the camera with depth-buffer turned off.
            * 9) Draw all visible nodes with **SLSceneView::draw3DGLAll**:
               * a) Draw nodes with meshes with opaque materials and all helper lines sorted by material.
               * b) Draw remaining opaque nodes without meshes.
               * c) Draw nodes with meshes with blended materials sorted by material and sorted back to front.
               * d) Draw remaining blended nodes (SLText, needs redesign).
               * e) Draw helpers in overlay mode (not depth buffered).
               * f) Draw visualization lines of animation curves.
               * g) Turn blending off again for correct anaglyph stereo modes.
            * 10) Draw right eye for stereo projection.
         * **SLSceneView::draw2DGL** is called for all 2D drawing
            * 1) The orthographic projection and viewport in screen space is set.
            * 2) A pseudo culling step for all nodes on the _root2D scene.
            * 3) SLSceneView::draw2DGLNodes:
               * all nodes in the _root2D scene node get drawn in orthographic projection.
            * 4) The ImGui::Render function is called that draws the ImGUI UI.
         * if Oculus stereo projection is used the Oculus frame buffer is drawn and swapped.
      * return true if either an update or a camera move occurred.
   * Swap the back buffer to the front.
   * Update the window title.
   * return the true if an immediate update is needed.