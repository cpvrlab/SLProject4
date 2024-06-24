\page blender-to-sl Blender to SLProject Export

\section intro Introduction
<p>
This page gives an overview of things to consider when preparing models created in the 3D software Blender for use in SLProject.
</p>

\section units Units (optional)
<p>
Units in SLProject are metric, 1 unit = 1 meter. For an easier workflow we recommend setting the units in blender to metric a well.
</p>
<img src="SLProject4/images/blender_units.png"/>
	
\section yup Working with Y up inside Blender (optional)
<p>
SLProject uses Y up, to work in blender with Y up you can do the following.
<ul>
	<li>Rotate your model in blender 90� around the X axis, so that positive Y in blender becomes the up axis.</li>
	<li>apply the rotation the the actual mesh by pressing ctrl+A and choose rotation from the context menu.</li>
</ul>
<b>[TODO: Add image showing the Y up rotation settings]</b><br />

You can now work with Y up inside of blender and don't have to rotate imported models by hand when importing them.
This step makes working with models and skeletons inside of SLProject easier. If you don't need to manipulate any of the data
you can of course work normally with Z up in blender and simply rotate the parent node in SLProject.
</p>

\section anims Animations
\subsection s1 Animation duration
<p>
When exporting animations make sure that your animation has a keyframe on its last frame. 
Blender will ignore the set 'End Frame' when exporting anduse the last keyframe as the length of the animation.
</p>

\subsection s2 Multiple animations for one model
<p>
Multiple animations. Blender doesn't support the exporting of multiple animations in a single file (Collada) at the time of this writing. This is why
it is required to export the file once for each different animation. However SLProject also doesn't support the importing of extra animations
in different files at the moment, so for now you can only import a single animation per model.
</p>

\section export Exporting
<p>
<b>[TODO: Add images showing exporting to collada file]</b><br />
<ol>
	<li>
		SLProject uses the Asset Import Library (Assimp), theoretically SLProject supports all formats that are supported by Assimp. However file outputs
		still differ based on what program exported them and what format they were exported in. So at the moment SLProject only supports the Collada file
		format. 
	</li>
	<li>
		To export your finished model go to File -> Export -> Collada. This will open a window where you choose your export location and export settings.
		If your model requires textures be sure to select the texture options visible in the screenshot. 
	</li>
	<li>
		Export the file.
	</li>
	<li>
		At the time of this writing Blender's Collada export options are lacking some vital settings. One of these is the choosing of the up axis. This means
		we have to set this part ourselfes by editing the exported Collada file by hand.
		Open the resulting Collada file with a text editor and search for the following line: "<up_axis>Z_UP</up_axis>" and replace Z_UP with Y_UP.
	</li>
</ol>
You can now import your model in SLProject, view the example scenes on how to do this.
</p>