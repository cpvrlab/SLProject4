\page example-particles Particle Systems
\htmlonly
<style>html{--content-maxwidth:auto}</style>
\endhtmlonly
The SLParticleSystem is the most complex derivative of the SLMesh class and allows the creation of highly
configurable particle systems (PS). All its parameters can be modified in the _Properties_ tab (see menu Info > Properties).
The implementation uses for rendering geometry shaders on systems that have OpenGL >= 4.1 or OpenGLES >= 3.2. All other
systems (mainly iOS and Emscripten with WebGL) use instanced rendering. The consecutive motion update is done with OpenGL
feedback buffers. For the torch fires in the following example scene we use one PS for the glow effect and one
for the flames. The center fire uses one PS for the glow, one for the flames, one for the sparks, one for the black,
and one for the white smoke. 

\htmlonly
Unfortunately, we can not embed externally hosted WebAssembly code on github.io.<br>
<a href="https://pallas.ti.bfh.ch/slproject?scene=76" target="_blank">Please open example in new tab</a>
<!--<iframe src="https://pallas.ti.bfh.ch/slproject?scene=76" width="100%" height="640" tabindex="0" style="border: 1px solid gray"></iframe>-->
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