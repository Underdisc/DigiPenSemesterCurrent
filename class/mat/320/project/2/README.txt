////////////////////////////////////////////////////////////////////////////////
// File:        README.txt
// Class:       MAT320
// Asssignment: Project 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-12
////////////////////////////////////////////////////////////////////////////////

To build and run the project, cd into the build/ directory and run make. A file
named dft.exe and fft.exe will be built.

The executables take the following input.

./dft.exe N filename
./fft.exe N filename

N is the number of samples that will be used from the complex values in the
"filename" file. The file should contain one complex value on each line. Here is
an example of a possible input file.

-- complex_values.txt --
3 + 4i
4 - i
8 + 2i
-i
------------------------

To compute the dft of the complex vlaues in the file, run the dft and fft 
programs in the following way.

./dft.exe 4 complex_values.txt
./fft.exe 4 complex_values.txt

These will take the first 4 complex values in complex_values.txt and compute
their dft. If there are 5 or more values in the file, they will be ignored
because the value of N is 4.

I have also included some sample input files and their output within the build
directory as well.




