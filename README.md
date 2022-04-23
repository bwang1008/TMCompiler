# TMCompiler

This project converts a very restricted subset of C++ code into a multi-tape Turing Machine representation.

In a Theory of Compuation course, we learn that "any program written in a Turing-Complete programming language, like C++, can be simulated by an equivalent Turing Machine". We learn that because Turing-Complete programming languages can simulate a Turing Machine, then the Turing-Church thesis implies these two computing models (the Turing-Complete language and the Turing Machine) are Turing equivalent, in the sense that they compute the same functions.

However, I find that the statement "for any given C++ program, I can build an equivalent Turing Machine that computes the same thing" is much more incredulous than the statement "I can write a C++ program that can simulate a Turing Machine". This project serves to demonstrate a watered-down version of the first statement by constructing an explicit multi-tape Turing Machine from a given program written in a restricted subset of C++.
 
Note to current Theory of Computation students: if your homework asks you to construct a Turing Machine that adds or multiplies two numbers, this project will not really help you! The formal description of the generated multi-tape Turing Machine will probably not fit on the page of your homework. 

This project is solely a "for fun" project.

## Installation
1. This project is meant to run through command-line on a Linux or Mac machine (sorry Windows!). Ensure you have the following installed: git, CMake, and a C++ compiler. Ensure you have a persistant internet connection.
2. Open a terminal and change to a directory to install the project. Then run each command below one-by-one:
```sh
git clone https://github.com/bwang1008/TMCompiler.git
cd TMCompiler
mkdir build
cd build
cmake -S .. -B .
make
```
This downloads the git repo from Github, creates and populates a `build` directory, and then finally compiles the source code in the project to generate two executables.
3. Type `ls` in the terminal and ensure there are two executables named `compile` and `simulate`.

## Running the Project
You can run one of the provided examples in `TMCompiler/programs/`, or you can write your own program. If you write your own, place it in the `TMCompiler/programs` folder, name it with a `.cpp` extension, and ensure that your file has a `int main()` function. Further notes on how to write such a program is detailed below. For now, suppose you want to run `TMCompiler/programs/fibonacci.cpp`, whose program requires an integer as input.

Be sure you are in the `TMCompiler/build/` directory, and that the executables `compile` and `simulate` exist.

1. Compile the program. This takes the form 
```sh
./compile <file.cpp>
```
In our example, this would be
```sh
./compile ../programs/fibonacci.cpp
```
The last line of output from compiling should say that the resulting multi-tape Turing Machine is stored in `../programs/fibonacci.json`. 

2. Optionally, simulate (run) the multi-tape Turing Machine. Do not do this if you think there is an infinite-loop in the source program! Run `./simulate --help` to see some options. To set an input of `18` for our `fibonacci` program, we would run the program as follows:
```sh
./simulate --input="18" ../programs/fibonacci.json
```

If you had compiled a program that did not require input, like `TMCompiler/programs/example.cpp`, then the program would be run with
```sh
./simulate ../programs/example.json
```

If your program requires multiple integers as input, use quotation marks around the space-separated integers, so the option would look something like `input="3 1 2 3"`.

## The Restricted C++
The restricted subset of C++, which I'll abbreviate as rC++, contains 
- `bool` data type
- `int` data type, but with arbitrary precision
- Arithmetical operators on ints: `+`,`-`,`*`,`/`,`%`,`^`,`-` for addition, subtraction, multiplication, floor division, modulo, exclusive-or, and the unary minus for negation. 
- Comparison operators on ints: `==`,`!=`,`<`,`<=`,`>`,`>=` for equals, not equals, less than, less than or equal to, greater than, and greater or equal to 
- Logical operators and comparison operators on bools: `==`,`!=`,`&&`,`||`,`^`,`!` for equals, not equals, AND, OR, exclusive-or, and the not operator
- Declaration and assignment of variables, like `int x = 999999999999999999999;`
- For-loops, while-loops, if statements
- Functions that can be recurisve or mutually recursive
- 3 provided functions:
    - `int nextInt()`, which reads in an integer from a tape designated as standard input
	- `void printInt(int x)` which prints an integer to a tape designated as standard output
    - `void printSpace()` which prints a space on the output tape. Should always be used after every call to `printInt`.
- A global int array `MEM` of theoretically infinite size, with values initialized to 0. You can get and set values in the array with statements like `MEM[3] = 5;` or `int x = MEM[3];`

Things that are NOT part of rC++ include, but are not limited to,
- floats or strings or any other types besides `int` and `bool`
- you cannot have something like `int[] x = MEM;` either
- classes or structs or objects
- preprocessor directives
- multiple files
- The operators `++`, `--`, `<<`, `>>`, `<<=`, `>>=`, `~`
- Casting
- Global variables (though you can use a value in `MEM` to act as a global variable)
- Function forward declarations
- malloc/free/new/delete
- do-while loop

The number of tapes in the multi-tape Turing Machine varies per program, but it will be at least 30 tapes. Writing a function that takes a lot of parameters is one way to increase the number of tapes used. While each tape is two-way infinite, each tape in the generated multi-tape Turing Machines will never go lower than index `-2`, so they are more like one-way infinite tapes. There are three symbols in the alphabet used by the multi-tape Turing Machine: `0`, `1`, and `_` (blank). A transition in a multi-tape Turing Machine with $$T$$ tapes is denoted using a regular expression that matches $$T$$ characters read from the $$T$$ tapes. This is used rather than specifying all possible values that the $$T$$ tapes could read to reduce space.  

## Extra notes
Integers will be in little-endian: there is a sign bit, and then the least-significant-bit 
to the most significant bit follow from left to right.

Generated Turing Machine will have 3 symbols: 0, 1, and space ( _ )

Number of tapes:
1 for INPUT  
1 for OUTPUT  
1 for Instruction Pointer stack  
1 for Instrution Pointer  
num bits in Instruction Pointer in 2's complement  
1 for MEM: Integer Array  
1 for Index of MEM  
1 for Variable Stack for parameters  
V for each declared variable, where V = max number of declared variables (including temp variables and parameters) across all functions  
1 for return values  

Current features:  
Conversion from source C++ code into assembly code.  
Conversion from assembly code into a MultiTapeTuringMachine  

Next ideas:  
Should rework Constants file  
Should rework CMake  
Optimizations  
Add conversion to and from JSON (using CMake)  
Change some exceptions to asserts  
Fix runner program once can convert to JSON  

Ideas for Optimization:  
Rework basic_add, isPos, isNeg, isZero to be in-place, to avoid unnecessary pushes and pops  
Incorporate "sideways" ip when changing ip. Should half the work of incrementing IP  

Future ideas:  
Add more features into what is allowed in the restricted C++, like strings and floats.  
Convert from multi-tape Turing Machine into a single-tape Turing Machine with only left and right shifts  

