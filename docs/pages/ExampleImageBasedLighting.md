\page example-ibl Image Based Lighting

The following scene shows 7 x 7 spheres with different metallic (vertical) and 
roughness (horizontal) material parameters. 
These parameters can be passed either by float values or as textures per pixel.
We implemented the standard *Physically Based Rendering (PBR)* material model introduced in 
[Unreal Engine 4 in 2013](https://de45xmedrsdbp.cloudfront.net/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf).
The ambient part is added by the HDRI environment map shown around the scene as a skybox.
The shaders for this type of SLMaterial are generated automatically in SLGLProgramGenerated.

\htmlonly
<iframe src="https://pallas.ti.bfh.ch/slproject?scene=15" width="960" height="640" frameBorder="0"></iframe>
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