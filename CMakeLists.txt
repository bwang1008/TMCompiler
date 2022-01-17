# guide from https://www.internalpointers.com/post/modern-cmake-beginner-introduction 

# required first line of CMakeList
cmake_minimum_required(VERSION 3.22.0)

# description of project, as well as executable name
project(tmCompilation
	VERSION 1.0
	DESCRIPTION "Convert Java-like code into a multi-tape Turing Machine"
	LANGUAGES CXX)

# generate executable "tmCompilation" from the following files
add_executable(tmCompilation	
	src/tm_definition/constants.hpp
	src/tm_definition/tape.hpp
	src/tm_definition/tape.cpp
	src/tm_definition/transition.hpp
	src/tm_definition/transition.cpp
	src/tm_definition/turing_machine.hpp
	src/tm_definition/single_tape_turing_machine.hpp
	src/tm_definition/single_tape_turing_machine.cpp
	src/tm_definition/runner.cpp)

# add properties to the executable: set c++=std_11, as well as compilation options -Wall,... for coding standards
target_compile_features(tmCompilation PRIVATE cxx_std_11)
target_compile_options(tmCompilation PRIVATE -Wall -Wextra -Wpedantic)

# to run: enter folder "build", type "cmake .." to run this CMakeList, 
# and then type "make help" to see the options in the Makefile
# Usually, would just run "make" to generate the executable
# then "./tmCompilation" to run (assuming no errors generated)
