Author: Connor Deakin
Professor: Dr. Pushpak Karnick
Class: CS 300
Project: Assignment 3
Date: 28/11/2017

I am listing the sections as listed they listed on the CS 300 syllabus.

a. Running the application
- When you compile, whether it's in release or debug, MAKE SURE TO USE 32-bit(x86).
- When you run the application, there is an ImGui window with a "Help" collapsing
  header at the top. Click that and it should explain everything needed to understand how to
  use the interface. The window is entirely resizable as well. If you can't see
  this section, maximize the entire Windows window and adjust the ImGui window.

a.5 Grading Criteria
- On startup, the program will create an image called "normal_map.png" in
  Project/Working/Resource/Texture. This is the normal map generated from the
  specular map.
- To view the t's & b's, look in the mesh tab of the main window. To apply
  texture & normal mapping, open the material editor and select the appropriate
  checkboxes.

b. Everything should work except the Gouraud and Blinn shaders. They are still
  there, but since they were not necessary for this assignment, I didn't worry
  bother worrying about them.

c. Completed
- Code compiles and links cleanly
- Information provided in correct format
- Scene setup implemented as described
- Appropriately coloured vector components
- Tangent debug drawing
- Bitangent debug drawing
- Normal map calculation
- Normal map edge case
- Normal map storage
- Normal map texture usage
- Tangent vector computed correctly
- Bitangent vector computed correctly
- Divide by zero handled correctly
- Tangent and Bitagent for vertices computed correctly
- T&B passed to GPU successfully
- TBN matrix implemented correctly
- Samplers implemented without errors
- Image to vector conversion
- Normal map transformation to view space
- Normal from texture used in lighting calculations


d. Not Completed
- Everything is completed for this assignment

e. File locations
- Source Code: Project/Source/
- Shader Code: Project/Working/Resource/Shader  <- The working directory is the
  actual working directory for the executable when it is run through visual studio.
  It just keeps everything organized.

f. EDISON LAB MACHINE: DIT2853US

g. 50-60~ish hours. This might be an underestimate, but this assignment took
  a very long time regardless. I am consistently failing to realize how much time
  it will take to do everything in a custom framework. Not only that, but I still needed
  to finish the texture mapping from the previous assignment. I had not finished
  it because I didn't have the time to do so.
