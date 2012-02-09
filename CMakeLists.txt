#-- Cmake configuration
cmake_minimum_required( VERSION 2.6 )
project (Motor)

#-- Build the library first
#TODO: Give Motor a main.cpp file or build it as a library.

#-- Then build each samples respectively
add_subdirectory(samples/Demo)
add_subdirectory(samples/DemoServer)
