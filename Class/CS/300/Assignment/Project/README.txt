Author: Connor Deakin
Professor: Dr. Pushpak Karnick
Class: CS 300
Project: Assignment 1
Date: 02/10/2017

I listing the sections as listed on the CS 300 syllabus.

a. Running the application
- When you compile, whether it's in release or debug, MAKE SURE TO USE 32-bit(x86).
- When you run the application, there is an ImGui window with a "Help" collapsing
  header at the top. Click that and it should explain everything needed to understand how to
  use the interface. The window is entirely resizable as well. If you can't see this
  section, maximize the entire Windows window and adjust the ImGui window.

b. The project will only load the bare minimum requirements for an OBJ file.
This means it will only load the files included with the project. I also included
the Stanford dragon (dragon.obj), because it looks cool.

c. Completed
- Code links and compiles cleanly.
- File open without error.
- File read in correctly.
- Face normals calculated and shown at face centers.
- Vertex normals calculated correctly (ignoring duplicates) and displayed at the vertices.
- GUI lets user change material values.
- GUI changes updates the rendering of the object correctly.
- Correct translation for all axes implemented.
- Correct Rotation for all axes implemented.
- Correct setup of VAO, Vertex Array, and Index Arrays
- Vertex position and vertex normals passed successfully as vertex attributes
- Index buffers used with correct offsets
- Correct usage of glDrawElements.
- No GL errors.

d. Not Completed
- One Function in MathFunctions.cpp. The Matrix3 variation of the Euler to Matrix
  function. I did not implement this because the Matrix4 variation is essentially
  the exact same thing and it had all the functionality needed for the rest of the
  assignment.
- Block Loading of OBJ files - I am not exactly sure what this is, but I am interested
  in increasing my model load times.
- Multiple lights - I did my own framework for this project. Everything in the project is mine except
  ImGui, the Zero Math library, and SDL. This made implementing everything that the other
  framework had on the first assignment pretty intensive in terms of work load, so I
  was not able to get everything in.

e. File locations
- Source Code: Project/Source/
- Shader Code: Project/Working/Resource/Shader  <- The working directory is the
  actual working directory for the executable when it is run through visual studio.
  It just keeps everything organized.

f. EDISON LAB MACHINE: DIT2853US

g. 20~ish hours. Most likely more. Implementing some framework features took quite
  a bit of time.

h. I did implement features that are required in the second assignment, but not for this assignment.
  I already wrote the phong shader with the ability to change all of the parameters in the
  ImGui window. I forgot to implement fog, but it's a very simple effect to add. The biggest
  thing I am missing is multiple lights, but since the next assignment allows for
  more than two lights, that will be one of the main features that I will implement
  for the next assignment.
