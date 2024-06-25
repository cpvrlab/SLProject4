\page example-gltf glTF File Format
\htmlonly
<style>html{--content-maxwidth:auto}</style>
\endhtmlonly
SLProject supports loading the [glTF model format](https://en.wikipedia.org/wiki/GlTF). This scene
showcases loading and rendering the damaged helmet sample by Khronos. Notice how the environment is
reflected onto the helmet. Note that it may take some time to load this scene since there is a lot of
data to be downloaded. 
We use the [assimp library](https://github.com/assimp/assimp) within SLAssetImporter for loading. 
The glTF file format supports also the *Physically Based Rendering (PBR)* material model. 
The shaders for this type of SLMaterial are generated automatically in SLGLProgramGenerated. 

\htmlonly
Unfortunately, we can not embed externally hosted WebAssembly code on github.io.<br>
<a href="https://pallas.ti.bfh.ch/slproject?scene=41" target="_blank">Please open example in new tab</a>
<!--<iframe src="https://pallas.ti.bfh.ch/slproject?scene=41" width="100%" height="640" tabindex="0" style="border: 1px solid gray"></iframe>-->
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