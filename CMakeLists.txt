# Steps to build with CMake:
# mkdir build
# cd build
# cmake -S .. -B .
# make

cmake_minimum_required(VERSION 3.12)

project(TMCompiler
	VERSION 2.0
	DESCRIPTION "Compile C-like programs into Turing Machines"
	LANGUAGES CXX
)

add_executable(tmc
	main.cpp
	TMCompiler/compiler/compiler.cpp
	TMCompiler/compiler/front_end/bnf_parser.cpp
	TMCompiler/compiler/front_end/earley_parser.cpp
	TMCompiler/compiler/models/grammar.cpp
	TMCompiler/utils/logger/logger.cpp
)

target_compile_options(tmc
	PRIVATE "-std=c++14"
)

target_include_directories(tmc
	PUBLIC "${CMAKE_SOURCE_DIR}"
)
