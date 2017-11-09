Author: Connor Deakin
Professor: Dr. Pushpak Karnick
Class: CS 300
Project: Assignment 2
Date: 27/10/2017

I am listing the sections as listed they listed on the CS 300 syllabus.

a. Running the application
- When you compile, whether it's in release or debug, MAKE SURE TO USE 32-bit(x86).
- When you run the application, there is an ImGui window with a "Help" collapsing
  header at the top. Click that and it should explain everything needed to understand how to
  use the interface. I added and changed a lot of things, so make sure to check it out.
  The window is entirely resizable as well. If you can't see this section, maximize
  the entire Windows window and adjust the ImGui window.

b. Everything should work. I expanded to OBJ loader to support loading other OBJ
  files, but there are still some bugs with it. However, it can load everything
  that is required. All loadable models can be found in Project/Working/Resource/Model.

c. Completed
- Code compiles and links cleanly
- Information provided in correct format
- Scene setup implemented as described
- Gouraud VS compiles without errors
- Gouraud FS compiles without errors
- Gouraud VS links without errors
- Gouraud FS links without errors
- Gouraud Program Object executed without errors
- Phong VS compiles without errors
- Phong FS compiles without errors
- Phong VS links without errors
- Phong FS links without errors
- Phong Program Object executed without errors
- Blinn VS compiles without errors
- Blinn FS compiles without errors
- Blinn VS links without errors
- Blinn FS links without errors
- Blinn Program Object executed without errors
- Ambient term used correctly in all three shaders
- Diffuse term used correctly in all three shaders
- Specular term used correctly in all three shaders
- Distance Attenuation term used correctly in all three shaders
- Fog term used correctly in all three shaders
- Spotlight term used correctly in all three shaders
- Directional Light term used correctly in all three shaders
- Multiple Lights used correctly in all three shaders
- No errors while loading textures from files
- Emissive term used correctly in calculations

d. Not Completed
- Samplers and texture mapping: I just didn't have the time to do it. This assignment
  took an extremely long time and I need to work on other things at this point.
  I intend on implementing it soon along with cleaning up a lot of my code. I did implement
  a texture loader using STB, but it is not currently in use.
- Console output for shader errors: I have my own custom error file logger that
  I use when checking the status of a shader compilation. These errors are
  written to Project/Working/assignment2.error. If other errors occur and the file
  doesn't get cleared, it can become cluttered.

e. File locations
- Source Code: Project/Source/
- Shader Code: Project/Working/Resource/Shader  <- The working directory is the
  actual working directory for the executable when it is run through visual studio.
  It just keeps everything organized.

f. EDISON LAB MACHINE: DIT2853US

g. 60-70~ish hours. This one took a really long time. I had to make a lot
  of extensions and changes to my framework as I worked on the assignment.
  This is definitely what took a majority of the time. Additionally, this assignment
  is just huge, especially when making a custom framework for it.  I actually
  implemented phong in the previous assignment, but this still took a large amount
  of time.

h. I implement the extra credit features that were mentioned in the assignment
  PDF (camera rotation). I also implemented full camera controls that can be
  used to move around in the environment.
