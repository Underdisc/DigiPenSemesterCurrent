CS300 Multitexturing Framework Sample
08 February 2015
Ben Henning
Tyler Pugmire
-------------------------------------

This sample operates exactly in the same capacity as the previous sample.
However, this one supports multitexturing. Simple change the shading mode in the
sample to Example Textures to render using the sample_texture shader provided
with the assignment 2 files. The slider underneath will blend between the
diffuse and specular TGA files provided with the assignment. This sample uses
the new Texture and TextureManager classes to achieve this. No sample code is
provided for how to do this in C++. It's similar to ShaderManager and there are
many comments in Texture and TextureManager explaining each various method.
Finally, feel free to look at sample_texture.frag/.vert for an idea of how that
shader works and how to work with textures in GLSL. It also gives you a slight
hint at how to compute UVs on the shader, which you will need to do using
methods of both cylindrical and spherical projection.

There are additional samples available in the 'additional' folder, including
both images and a video of some of the features of the assignment. If a feature
was not covered in one of these images or the video, it is expected for you to
infer what it should look like as you develop the application (e.g.
phong lighting/gouraud shading).
