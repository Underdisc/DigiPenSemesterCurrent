Author: Connor Deakin
Professor: Dr. Pushpak Karnick
Class: CS 300
Project: Assignment 2
Date: 28/11/2017

I am listing the sections as listed they listed on the CS 300 syllabus.

a. Running the application
- When you compile, whether it's in release or debug, MAKE SURE TO USE 32-bit(x86).
- When you run the application, there is an ImGui window with a "Help" collapsing
  header at the top. Click that and it should explain everything needed to understand how to
  use the interface. I added and changed a lot of things, so make sure to check it out.
  The window is entirely resizable as well. If you can't see this section, maximize
  the entire Windows window and adjust the ImGui window.

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
