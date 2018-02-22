PortMidi can be downloaded from
    http://portmedia.sourceforge.net/portmidi/
For your convenience, I have compiled the latest version for use on MS Windows.


If you have administrative privileges on the machine that you are using, you can install the files in the following directories.

(1) portmidi.h
    Install this in the directory:
      C:\Program Files\Microsoft Visual Studio 14.0\VC\include

(2) portmidi.lib
    Install this in the directory:
      C:\Program Files\Microsoft Visual Studio 14.0\VC\lib

(3) portmidi.dll
    Install this in the directory:
      C:\WINDOWS
    (or any directory in the DLL search path)

If you do not have administrative privileges, you can do the following.
- Copy the above 3 files into the directory that you are working in.  
- Compile your code from the Visual Studio Command Prompt using
      cl /EHsc /I. <file(s)> portmidi.lib

