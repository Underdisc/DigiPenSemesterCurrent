To make your life a little easier, I've created a *simple* API for audio controls.  Here is the Windows implementation (I've also included an inferior platform generic version -- buyer beware).  For convenience, I've included the source code as well as a compiled version.

(1) Control.h   -- header file needed
(2) Control.cpp -- Win32 implementation
(3) Control.lib -- Compiled static library

To compile a program that uses the API, type

  cl /EHsc <file(s)> Control.cpp user32.lib

Or,

  cl /EHsc <files(s)> Control.lib user32.lib

on the Visual Studio Command Prompt.  Note that in either case, the 'user32.lib' (Win32) library must be linked with.
