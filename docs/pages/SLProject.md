\mainpage SLProject
\htmlonly
<style>html{--content-maxwidth:auto}</style>
\endhtmlonly

<p>
Welcome to the SLProject. SL stands for Scene Library. It is developed at the 
<a href="http://www.bfh.ch/en/studies/bachelor/engineering_and_information_technology/information_technology.html">
Berne University of Applied Sciences (BFH)</a> and is used for student projects in the 
<a href="https://www.cpvrlab.ti.bfh.ch/">cpvrLab</a>. 
The various applications show what you can learn in three semesters about 3D computer graphics in 
real time rendering and ray tracing. The framework is built in C++ and OpenGL ES and can be built for Windows, 
Linux, macOS (Intel & arm64), Android, Apple iOS and for WebAssembly enabled browsers. 
The framework can render alternatively with Ray Tracing and Path Tracing which provides in addition 
high quality transparencies, reflections and soft shadows.
</p>

<p>
The SLProject is hosted online on <a href="https://github.com/cpvrlab/SLProject4">GitHub</a>.
</p>
<p>
The source code is provided under the <a href="http://www.gnu.org/licenses/gpl.html">GNU General Puplic License (GPL)</a>. 
The code is provided without any warranties whether expressed or implied.
</p>

<h3>Read the following wiki page for more information:</h3>
<ul>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki">Wiki Home</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/How-to-clone-SLProject">How to get SLProject</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/SLProject-Features">SLProject Features</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Version-History">SLProject Version History</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-on-Windows-with-VisualStudio">Build instructions for MS Visual Studio</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-with-QtCreator">Build on any OS with QtCreator</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-for-Android">Build for Android with Android Studio.</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-on-MacOS-with-XCode-for-MacOS">Build for MacOS with Apple XCode</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-on-MacOS-with-XCode-for-iOS">Build for iOS with Apple XCode</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-on-Ubuntu-Linux">Build on Ubuntu Linux</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Build-for-the-web-with-Emscripten">Build for the Web with Emscripten</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Minimal-OpenGL-Apps">Minimal OpenGL Applications</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Minimal-OpenCV-Apps">Minimal OpenCV Applications</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Folder-Structure">Repository folder structure</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/Coding-Style-Guidelines">Coding Style Guidelines for Contributors</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/How-to-Generate-the-Documentation">How to generate the HTML Documentation using Doxygen</a></li>
<li><a href="https://github.com/cpvrlab/SLProject4/wiki/SLProject-Demo-App:-Data-Protection-Declaration">Data Protection Declaration</a></li>
</ul>

<h3>Introduction:</h3>
<p>
Read the \subpage introduction for an overview of the framework.<br>
Read \subpage on-paint for an overview of how on frame gets rendered.<br>
Read \subpage emscripten for an overview of how SLProject runs in the browser.
</p>

<h3>Contributors:</h3>
<p>
Contributors since 2005 in alphabetic order:
Martin Christen, Jan Dellsperger, Manuel Frischknecht, Luc Girod, Michael Goettlicher, Michael Schertenleib, Stefan Thoeni, Timo Tschanz, Marc Wacker, Marino von Wattenwyl, Pascal Zingg
</p>

<h3>Framework Dependencies:</h3>
The framework uses beside OpenGL the following external libraries that are also included in the SLProject repository:
<ul>
    <li><a href="http://assimp.org/">assimp</a> for 3D file formats loading.</li>
    <li><a href="https://emscripten.org//">emscripten</a> for the WebAssembly build for browsers.</li>
    <li><a href="https://github.com/skaslev/gl3w">GL3W</a> for OpenGL function binding.</li>
    <li><a href="http://www.glfw.org/">GLFW</a> for window and OpenGL context creation.</li>
    <li><a href="https://developers.google.com/mediapipe">Mediapipe</a> for Google's machine learning framework.</li>
    <li><a href="http://www.opencv.org/">OpenCV</a> for image processing.</li>
    <li><a href="http://www.opengl.org/">OpenGL</a> platform independent realtime rendering.</li>
    <li><a href="http://www.openssl.org/">OpenSSL</a> for secure sochet layer networking in HTTPS</li>
    <li><a href="https://github.com/ocornut/imgui">ImGUI</a> for an easy to use GUI.</li>
    <li><a href="https://midcdmz.nrel.gov/solpos/spa.html">SPA</a> Solar Positioning Algorithm</li>
    <li><a href="https://github.com/nothings/stb">stb_image</a> for image file I/O.</li>
    <li><a href="https://zlib.net">zlib</a> for data compression and decompression</li>
</ul>

<h3>Support:</h3>
<ul>
   <li>The code is provided without any warranties whether expressed or implied.</li>
   <li>No support can be provided on installation, compiler or build issues. The
      framework was tested on several machines using MS Visual Studio 2017 as
      well as on Mac OSX using XCode with GCC or LLVM compiler.
   </li>
   <li>The code is provided under <a href="http://opensource.org/licenses/GPL-3.0">GNU General Puplic License (GPL)</a>. 
      This means in general, that you must provide your code as well under GPL if you 
      use this code.
   </li>
   <li>Feedback always welcome.</li>
</ul>

<p>
Authors: marcus.hudritsch@bfh.ch<br>
Date: June 2023<br>
Copyright (c): 2002-2023 Marcus Hudritsch, Kirchrain 18, 2572 Sutz, Switzerland
</p>
