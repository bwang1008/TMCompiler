# TMCompiler

This project implements a compiler that translates a program with C-like syntax into a multi-tape Turing Machine.

The Turing-completeness of a programming language implies the set of computable functions that can be computed by the programming language, is the same as that of a Turing Machine. In other words, for any well-formed program in the programming language, there is an equivalent Turing Machine that computes the same function! This project demonstrates this statement by explicitly constructing a multi-tape, 3-symbol Turing Machine from a given program written with C-like syntax.

## Installation
1. This project is meant to run through command-line on a Linux or Mac machine (sorry Windows!). Ensure you have the following installed: git, CMake, Make, and a C++ compiler. You can check this through `git --version`, `cmake --version`, `make --version`, and `g++ --version` (if you use `g++` for your C++ compiler).

2. Open a terminal and change to a preferred directory to install the project. Then run each command below one by one:
```sh
git clone https://github.com/bwang1008/TMCompiler.git
cd TMCompiler
make
```

This downloads the git repository from Github and compiles the source code in the project to generate executables. The final step may take a while.

## Project Structure
- `TMCompiler/compiler/` is the part responsible for parsing source code and translating to a Turing Machine specification
	- `models/` contains data-structure representations of concepts related to grammars and parsing, such as tokens and rules
	- `frontend/` contains helper functions for parsing BNF files and for implementing an Earley Parser
- `TMCompiler/utils/` contains helper libraries for logging, unittesting, and parsing command-line options
- `TMCompiler/config/` contains configuration files for the project, such as the BNF specification of the input source program
- `TMCompiler/tests/` contains testcases to ensure features do not break

This project is solely a for-fun project! Any comments, questions, bug reports, or suggestions are welcome!
