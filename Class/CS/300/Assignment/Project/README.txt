Author: Connor Deakin
Professor: Dr. Pushpak Karnick
Class: CS 300
Project: Assignment 4
Date: 08/12/2017

I am listing the sections as listed they listed on the CS 300 syllabus.

a. Running the application
- When you compile, whether it's in release or debug, MAKE SURE TO USE 32-bit(x86).
- When you run the application, there is an ImGui window with a "Help" collapsing
  header at the top. Click that and it should explain everything needed to understand how to
  use the interface. The window is entirely resizable as well. If you can't see
  this section, maximize the entire Windows window and adjust the ImGui window.

a.5 Grading Criteria
- Since you're grading, you're likely only interested in the features required
  for assignment 4.
- The application will start with a single rotating light. If you would like to
  add more lights, open the light editor and increase the number of lights.
- Almost everything needed for the assignment can be found in the material editor.
- Environment Mapping is enabled by default.
- The Environment Factor parameter is a 0 to 1 value. At 1, the material will
  be displayed entirely with environment reflection/refraction. At 0, a standard
  phong lighting model is used. Values in-between will be a linear interpolation
  of those color values.
- Refraction Index is the refraction index of the material.
- After enabling chromatic abberation, you must make the chromatic offset parameter
  positive in order to view the chromatic abberation effect. The chromatic offset
  simply describes the change in the refraction index for red and blue values.
  ------
  red refraction index = refraction index - chromatic offset
  green refaction index = refration index
  blue refraction index = refraction index + chromatic offset
  ------
- The last parameter allows you to choose how much reflection / refraction is
  visible when fresnel reflection is not enabled.

b. Everything should work except the Gouraud and Blinn shaders. They are still
  there, but since they were not necessary for this assignment, I didn't worry
  bother worrying about them.

c. Completed
- Code compiles and links cleanly
- Skybox rendered correctly
- Skybox rendered correctly with 2D textures
- Camera setup to render dynamically
- 6 textures rendered correctly
- 6 textures rendered correctly on skybox sides
- Rotating spheres rendered correctly on textures
- Skybox and object rendered with correct depth operations
- Clamping to edge used correctly
- 6 Textures passed correctly to fragment shader
- R & T calculated in fragment shader
- Cube map projector function correctly implemented
- UVs correctly calculated
- UI to toggle reflection
- UI to toggle refraction
- UI to toggle Fresnel (Not entirely sure if this is correct since I do not
    full understand the calculation. It looks correct though).
- UI to toggle emissive combination
- UI to change refraction index

d. Not Completed
- Everything is completed for this assignment.

e. File locations
- Source Code: Project/Source/
- Shader Code: Project/Working/Resource/Shader  <- The working directory is the
  actual working directory for the executable when it is run through visual studio.
  It just keeps everything organized.

f. EDISON LAB MACHINE: DIT2853US

g. 30-35 hours. Not as bad as the last assignment for me, but still a lot time. I was
  also very behind when I turned in the last assignment, so this seems reasonable.

h. I implemented chromatic abberation and normal mapping for this assignment.
  You can enable normal mapping in the material menu and it will have an effect
  on the environment reflection / refraction effects.
