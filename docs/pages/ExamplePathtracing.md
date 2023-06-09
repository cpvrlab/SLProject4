\page example-pathtracing Path Tracing

Alternatively you can render a scene with path tracing using the menu *Renderer > Path Tracing*.
The path tracer is optimized for the Blinn-Phong reflection model extended with reflection and refraction coefficient.
By default, the resolution 0.5 times the window resolution. You can adjust the path tracing setting over the menu *PT*.
On the web there is no inter-frame update of the scene. Depending on the complexity you have to be patient a few seconds.
The path tracer is multithreaded and uses a combination bounding boxes and regular voxel grid for fast ray-triangle intersection.
The path tracer is implemented in the classes SLPathtracer and SLRay.

\htmlonly
<iframe src="https://pallas.ti.bfh.ch/slproject?scene=65" width="1024" height="640" frameBorder="0"></iframe>
\endhtmlonly

General help:
<ul>
  <li>Click and drag the left mouse button to rotate the scene.</li>
  <li>Click and drag the middle mouse button to move sidewards/up-down.</li>
  <li>Roll the mouse-wheel to move forward/backward.</li>
  <li>Double click with the left mouse button to select an object.</li>
  <li>Click the right mouse button to open the context menu.</li>
  <li>Open and dock additional windows from the menu <em>Infos</em>.</li>
  <li>See more example scenes over the menu <em>File > Load Test Scene</em></li>
</ul>