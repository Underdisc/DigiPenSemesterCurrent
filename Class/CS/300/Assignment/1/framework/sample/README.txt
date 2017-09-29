CS300 Assignment 1 Framework Sample
13 January 2015
Ben Henning
-----------------------------------

To run the sample, simply double-click and run sample.exe within this directory.
It will automatically link to the assets folder one directory up. Just like your
assignment should, it will automatically try to load cube.obj from
assets/models. If you have not yet created this OBJ file, it will issue an error
message to the console. Feel free to try loading the provided horse or bunny
model; those should work. If there are errors in your shader.frag or shader.vert
in your assets folder, then this sample will be affected by those errors.
Reloading shaders is an implemented feature in this sample, unlike the provided
framework. Implementing reloading shaders is not necessary for this assignment,
though helpful in both this and future assignments.

Notice the following features:
1. The perspective matrix remains correct upon resizing the window.
2. Vertex normals and face normals debug modes are supported. Note that the
   option to render both simultaneously is NOT necessary to implement for this
   assignment.
3. Changing the model position does correctly move the model in world space.
4. Changing the model's X, Y, or Z rotations does change its orientation in
   world space.
5. Changing the ambient or diffuse color terms of the object's Material
   correctly changes its colors affected by the lighting computations.
6. Changing the directions, ambient, or diffuse color terms of each lights
   alters the lighting on the model as would be expected. Note how the lighting
   is done in world space. The actual lighting logic performed in shader code
   does not need to be changed for this assignment.
