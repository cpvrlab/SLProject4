\page example-raytracing Ray Tracing
\htmlonly
<style>html{--content-maxwidth:auto}</style>
\endhtmlonly
Alternatively you can render a scene with ray tracing using the menu *Renderer > Ray Tracing*.
The ray tracer is optimized for the Blinn-Phong reflection model. 
By default, the resolution 0.5 times the window resolution. You can adjust the ray tracing setting over the menu *RT*.
On the web there is no inter-frame update of the scene. Depending on the complexity you have to be patient a few seconds. 
The ray tracer is multithreaded and uses a combination bounding boxes and regular voxel grid for fast ray-triangle intersection.
The ray tracer is implemented in the classes SLRaytracer and SLRay.

\htmlonly
Unfortunately, we can not embed externally hosted WebAssembly code on github.io.<br>
<a href="https://pallas.ti.bfh.ch/slproject?scene=66" target="_blank">Please open example in new tab</a>
<!--<iframe src="https://pallas.ti.bfh.ch/slproject?scene=66" width="100%" height="640" tabindex="0" style="border: 1px solid gray"></iframe>-->
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