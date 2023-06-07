How one frame gets painted
============

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
      * SLSceneView::onPaint called for every sceneview:
         * SLSceneView::draw3DGL
            * SLCamera::camUpdate updates any camera animation (smooth transitions)
            * All buffers are cleared (color, depth and Oculus frame buffer)
            * **Camera Settings**:
               * SLCamera::setProjection sets the projection
                 (perspective, orthographic or one of the stereo projections) and viewport.
                 On stereo the projection for the left eye is set.
               * SLCamera::setView applies the view transform.
            * **Frustum Culling**:
               * SLCamera::setFrustumPlanes set the cameras frustum planes according to the
                 current projection and view transform.
               * SLNode::cullRec called on the root node:
                  * All nodes are checked if they are visible.
                     * All visible nodes without transparencies (opaque) are added to the _opaqueNodes vector.
                     * All visible nodes with transparencies are added to the _blendNodes vector.
            * SLSceneView::draw3DGLAll:
               * Blending is turned off and the depth test on
               * SLSceneView::draw3DGLNodes is called for every node in the _opaqueNodes vector:
                  * The view matrix is applied to the modelview matrix
                  * The nodes world matrix is applied to the modelview matrix
                  * SLMesh::draw is called on all meshes of the node:
                     * 1) Apply the drawing bits
                     * 2) Apply the uniform variables to the shader
                        * 2a) Activate a shader program if it is not yet in use and apply all its material parameters.
                        * 2b) Pass the modelview and modelview-projection matrix to the shader.
                        * 2c) If needed build and pass the inverse modelview and the normal matrix.
                        * 2d) If the mesh has a skeleton and HW skinning is applied pass the joint matrices.
                     * 3) Build the vertex attribute object once
                     * <span style="color:red"><strong>4) Finally do the draw call</strong></span>
                     * 5) Draw optional normals & tangents
                     * 6) Draw optional acceleration structure
                     * 7) Draw optional the selected node
               * SLSceneView::draw3DGLLines: for every node in the _opaqueNodes vector:
                 * The view matrix is applied to the modelview matrix
                 * If the drawbit for viewing the AABBs is set SLAABBox::drawWS draws it.
                 * If the drawbit for viewing the axis is set SLAABBox::drawAxisWS draws it.
               * SLSceneView::draw3DGLLines: for every node in the _blendNodes vector the same as above.
               * Blending is turned on and the depth test off.
               * The nodes of the _blendNotes get sorted by depth for correct transparency blending.
               * SLSceneView::draw3DGLNodes is called for every node in the _blendNodes vector (same as above).
               * Blending is turned off and the depth test on again.
            * SLSceneView::draw3DGLAll:
               * ... (the same but for the right eye of stereo projections)
         * SLSceneView::draw2DGL is called for all 2D drawing
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