# guide from https://www.internalpointers.com/post/modern-cmake-beginner-introduction 

# required first line of CMakeList
cmake_minimum_required(VERSION 3.16.3)

# description of project, as well as executable name
project(TMCompiler
	VERSION 1.0
	DESCRIPTION "Convert C++-like code into a multi-tape Turing Machine"
	LANGUAGES CXX)

# generate executable "compile" from the following files
add_executable(compile
	TMCompiler/utils/constants.hpp
	TMCompiler/utils/helper.hpp
	TMCompiler/utils/helper.cpp
	TMCompiler/utils/json.hpp
	TMCompiler/tm_definition/tape.hpp
	TMCompiler/tm_definition/tape.cpp
	TMCompiler/tm_definition/transition.hpp
	TMCompiler/tm_definition/transition.cpp
	TMCompiler/tm_definition/multi_tape_turing_machine.hpp
	TMCompiler/tm_definition/multi_tape_turing_machine.cpp
	TMCompiler/compilation/compile.hpp
	TMCompiler/compilation/compile.cpp
	TMCompiler/compilation/multi_tape_builder.hpp
	TMCompiler/compilation/multi_tape_builder.cpp
	TMCompiler/compilation/unit1.hpp
	TMCompiler/compilation/unit1.cpp
	TMCompiler/compilation/unit2.hpp
	TMCompiler/compilation/unit2.cpp
	TMCompiler/runner/compileRunner.cpp
	)

# generate executable "simulate" from the following files
add_executable(simulate
	TMCompiler/utils/constants.hpp
	TMCompiler/utils/helper.hpp
	TMCompiler/utils/helper.cpp
	TMCompiler/utils/json.hpp
	TMCompiler/tm_definition/tape.hpp
	TMCompiler/tm_definition/tape.cpp
	TMCompiler/tm_definition/transition.hpp
	TMCompiler/tm_definition/transition.cpp
	TMCompiler/tm_definition/multi_tape_turing_machine.hpp
	TMCompiler/tm_definition/multi_tape_turing_machine.cpp
	TMCompiler/runner/simulateRunner.cpp
	)

# copy the file TMCompiler/compilation/lib.cpp (not for usual compilation) to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TMCompiler/compilation/lib.cpp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# include the root TMCompiler folder so include statements work
target_include_directories(compile PUBLIC ${PROJECT_SOURCE_DIR})

target_include_directories(simulate PUBLIC ${PROJECT_SOURCE_DIR})

# add properties to the executable: set c++=std_14, as well as compilation options -Wall,... for coding standards
target_compile_options(compile PRIVATE -std=c++14 -O3 -Wall -Wextra -Wpedantic)

target_compile_options(simulate PRIVATE -std=c++14 -O3 -Wall -Wextra -Wpedantic)

# to run: enter folder "build", type "cmake -S .. -B ." to run this CMakeList, 
# and then type "make help" to see the options in the Makefile
# Usually, would just run "make" to generate the executable
# then "./compile" to run (assuming no errors generated)
