////////////////////////////////////////////////////////////////////////////////
// File:        README.txt
// Class:       MAT320
// Asssignment: Project 3
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-11-04
////////////////////////////////////////////////////////////////////////////////

To build and run the project, cd into the build/ directory and run make. This
will build 5 programs.

bitrev.exe
genrandoms.exe
dft.exe
fft.exe
fft2.exe

The three programs made for this assignment are bitrev.exe, fft2.exe, and
genrandoms.exe. genrandoms.exe will generate N "somewhat" random complex
numbers. dft.exe and fft.exe are from the previous assignment, but I included
them in this submission since they were used to test the timings of the 3
dft implementations.

The executables take the following input.

./bitrev.exe N
N - The number of values in the array that will be bit reversed.

./genrandoms.exe N
N - The number of random complex numbers to generate.

./dft.exe N filename
./fft.exe N filename
./fft2.exe N filename
N - The number of samples to take from the complex number file.
filename - The file where the complex numbers are stored.

== Computing DFT ===============================================================

Here is an example of what a complex number file will look like.
-- complex_values.txt --
3 + 4i
4 - i
8 + 2i
-i
------------------------

To compute the dft of the complex values in the file, run the dft, fft, and fft2
programs in the following way.

./dft.exe 4 complex_values.txt
./fft.exe 4 complex_values.txt
./fft2.exe 4 complex_values.txt

These will take the first 4 complex values in complex_values.txt and compute
their dft. If there are 5 or more values in the file, they will be ignored
because the value of N is 4.

To generate some sample input files, just run genrandoms.exe and redirect the
output into a file.

== Find Timings  ===============================================================

In the build directory, there is also a file named timings.sh. After running
make in the build directory, this shell script can be invoked to see the
amount of time taken for dft.exe, fft.exe, and fft2.exe to compute the dft of
N complex numbers. Just run the following and all of the timing information will
show up as output when the programs finish executing.

./timings.sh N

Make sure N is a power of 2. No error will be printed out because the shell
script redirects all of the output to files.
