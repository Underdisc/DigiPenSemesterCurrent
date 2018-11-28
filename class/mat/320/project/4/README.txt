////////////////////////////////////////////////////////////////////////////////
// File:        README.txt
// Class:       MAT320
// Asssignment: Project 4
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-11-26
////////////////////////////////////////////////////////////////////////////////

To build and run the project, cd into the build/ directory and run make. This
will build lowpass.exe. The executable takes the following input.

./lowpass.exe a n filename
a - The lowpass filter factor.
n - The number of times the lowpass filter will be applied.
filename - The wave file containing the audio data that the lowpass filter will
           be applied to.

When successful, lowpass.exe will create a file named output.wav. This will
contain the audio data obtained by applying the specified lowpass filter n
times.