\page example-lod Level of Detail
The following scene shows 2500 corinthian columns with each 83k triangles in their highest resolution. 
With 3 levels of detail the amount of geometry is reduced depending on the size of the bounding rectangle in screen space. 
In addition, we do automatically a view frustum culling to optimize rendering performance.
See the menu *GL* for additional visualizations for the OpenGL renderer.
The scene uses also cascaded shadow mapping with the current time im Switzerland. 
You can adjust the date and time with the dialogue *Info > Date-Time*.

\htmlonly

Unfortunately, we can not embed externally hosted WebAssembly code on github.io.<br>
<a href="https://pallas.ti.bfh.ch/slproject?scene=84" target="_blank">Please open example in new tab</a>
<!--<iframe src="https://pallas.ti.bfh.ch/slproject?scene=84" width="100%" height="640" tabindex="0" style="border: 1px solid gray"></iframe>-->
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