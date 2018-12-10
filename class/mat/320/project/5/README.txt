////////////////////////////////////////////////////////////////////////////////
// File:        README.txt
// Class:       MAT320
// Asssignment: Project 5
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-5
////////////////////////////////////////////////////////////////////////////////

To build and run the project, cd into the build/ directory and run make. This
will build pluck.exe. The executable takes the following input.

./pluck.exe f
f - The root frequency that the major scale will be played from.

When successful, pluck.exe will create a file named pluck.wav. This will
contain the audio data obtained by applying a changing plucked string filter to 
pulses of noise, producing an ascending major scale.