////////////////////////////////////////////////////////////////////////////////
// File:        README.txt
// Class:       MAT320
// Asssignment: Project 1
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-09-28
////////////////////////////////////////////////////////////////////////////////

This is just a quick introduction on how to compile and run the program. In the
build directory there is a makefile to quickly build the project. This should
work right out of the box given the machine has g++ or clang++. The makefile
is currently set up to build with g++, but it can be changed to clang. Just
search for CC in the makefile and change the CC and LINKER variables to the
following.

CC=clang++
LINKER=clang++

If the makefile is not working, just run one of the following from the build
directory.

g++ -std=c++11 ../src/main.cc ../src/Complex.cc -o prog.exe
clang++ -std=c++11 ../src/main.cc ../src/Complex.cc -o prog.exe

To test the program, all of the arguments specified in the handout will be
passed in as command line arguments. For example, to run part 3 of the project
with an input of 2 and two files named complex_lhs.txt and complex_rhs.txt, the
following command should be used.

./prog.exe 3 2 complex_lhs.txt complex_rhs.txt

Here is a list of how to run all of the different parts of the assignment. The
variable names correspond to how they are named on the project handout.

./prog.exe 1 N x filename
./prog.exe 2 N k
./prog.exe 3 N filename filename
./prog.exe 4 N filename

Note
The complex implementation is from some time ago. I made it last year because I
using it to construct a water simulation that used an fft for computing the
water surface. Most of the complex implementation came from that project, but I
did add the Complex::Polar() function to make the assignment easier.
