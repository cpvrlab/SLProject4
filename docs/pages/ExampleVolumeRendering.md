\page example-volume-rendering Volume Rendering

\htmlonly
<style>html{--content-maxwidth:auto}</style>
\endhtmlonly
Volume Rendering of an MRI angiography dataset. In the material properties you can adjust the transfer function for
color and transparency points that transform the MRI intensities into color and transparency. Volume rendering is
realized with a special fragment shader that performs a ray cast through the MRI volume.

\htmlonly
Unfortunately, we can not embed externally hosted WebAssembly code on github.io.<br>
<a href="https://pallas.ti.bfh.ch/slproject?scene=46" target="_blank">Please open example in new tab</a>
<!--<iframe src="https://pallas.ti.bfh.ch/slproject?scene=46" width="100%" height="640" tabindex="0" style="border: 1px solid gray"></iframe>-->
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