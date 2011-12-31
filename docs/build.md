Build instructions
==================

Linux
---
First you'll need SDL, OpenGL, CMake and glew installed.
$ sudo pacman -S sdl glew cmake

We create a new directory to isolate object files from the sources.
We move to this directory.
Then we generate a solution for our operating system (makefiles for linux)
Then we compile everything up with that makefile.

$ mkdir build
$ cd build/
$ cmake ..
$ make

If you're still inside the buid folder, you may now run the Demo generated:

$ ./samples/Demo/Demo

Windows
---
First you'll need CMake installed.
> http://cmake.org/cmake/resources/software.html
(cmake-2.8.7-win32-x86.exe recommended)

> Run the CMake-gui program; it's much easier to understand than plain old command line.
"Where is the source code"; browse to the folder on your computer.
"Where to build the binaries"; any directory you want, You will be asked to create a new one if the path selected doesn't already exist.
> Click the "Configure" button
Choose the IDE / compiler you are using. Most commons are Code::Blocks and Visual C++ 2010.
> Click the "Generate" button
A project will be generated for your IDE / compiler. You can now start using it :)

The default project built should be "Demo".

SDL
---
For Mac or Linux: install the libsdl package

For Windows: go to http://www.libsdl.org/ and go to the download page. The link is at the left called 'SDL 1.2'.
Then download the file at 'Development Libraries' for Visual C++ 6.0 (this works with Visual C++ 2010).
Unzip the contents of the include directory to /build/vc10/include/SDL/ so that you get /build/vc10/include/SDL/SDL.h
(You might have to create the SDL folder manually)
