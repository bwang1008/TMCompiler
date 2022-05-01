# TMCompiler

This project converts a very restricted subset of C++ code into a multi-tape Turing Machine representation.

In a Theory of Compuation course, we learn that "any program written in a Turing-Complete programming language, like C++, can be simulated by an equivalent Turing Machine". We learn that because Turing-Complete programming languages can simulate a Turing Machine, then the Turing-Church thesis implies these two computing models (the Turing-Complete language and the Turing Machine) are Turing equivalent, in the sense that they compute the same functions.

However, I find that the statement "for any given C++ program, I can build an equivalent Turing Machine that computes the same thing" is much more incredulous than the statement "I can write a C++ program that can simulate a Turing Machine". This project serves to demonstrate a watered-down version of the first statement by constructing an explicit multi-tape Turing Machine from a given program written in a restricted subset of C++.
 
The current project version is 1.0.

This project is solely a "for fun" project.

## Installation
1. This project is meant to run through command-line on a Linux or Mac machine (sorry Windows!). Ensure you have the following installed: git, CMake, Make, and a C++ compiler. Ensure you have a persistant internet connection.
2. Open a terminal and change to a directory to install the project. Then run each command below one-by-one:
```sh
git clone https://github.com/bwang1008/TMCompiler.git
cd TMCompiler
mkdir build
cd build
cmake -S .. -B .
make
```
This downloads the git repository from Github, creates and populates a `build` directory, and then finally compiles the source code in the project to generate two executables. The last command make take some time to complete.
3. Type `ls` in the terminal and ensure there are two executables named `compile` and `simulate`.

## Running the Project
You can run one of the provided examples in `TMCompiler/programs/`, or you can write your own program. If you write your own, place it in the `TMCompiler/programs` folder, name it with a `.cpp` extension, and ensure that your file has a `int main()` function. For now, suppose you want to run `TMCompiler/programs/fibonacci.cpp`, whose program requires an integer as input.

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

If your program requires multiple integers as input, use quotation marks around the space-separated integers, so the option would look something like `--input="3 1 2 3"`.

## Example Run
Suppose I want to run the provided fibonacci program. Then my compilation looks like
```sh
.../TMCompiler/build$ ./compile ../programs/fibonacci.cpp
Library file found
Initialization complete
Finished building
tapes:
tape input = 0
tape output = 1
tape ipStack = 2
tape ip = 3
tape ipSideways = 4
tape paramStack = 16
tape bitIndex = 17
tape bits = 18
tape variables = 19
tape rax = 31
Compile successful

14335 states
32 tapes
Start at node 0 and ends at 1
21497 transitions

Results written to file ../programs/fibonacci.json
```
Afterwards, I can simulate the multi-tape Turing Machine with
```sh
.../TMCompiler/build$ ./simualte --input="15" ../programs/fibonacci.json
File successfully read
JSON to MTTM conversion successful

Begin simulating:
Finished step 50000
Finished step 100000

Final:
Tape 0: 6: [01111]
Tape 1: 0: []
Tape 2: -2: [_11111111110__10001001100__10000101101__01000000010]
Tape 3: 0: [111111111111]
Tape 4: 0: [1]
Tape 5: 0: [1]
Tape 6: 0: [1]
Tape 7: 0: [1]
Tape 8: 0: [1]
Tape 9: 0: [1]
Tape 10: 0: [1]
Tape 11: 0: [1]
Tape 12: 0: [1]
Tape 13: 0: [1]
Tape 14: 0: [1]
Tape 15: 0: [1]
Tape 16: 0: [___1111__00001_0010111]
Tape 17: 0: [0]
Tape 18: 0: []
Tape 19: -2: [_1111__1111__00001__00001]
Tape 20: -2: [_0100011001_1101__1111__1111]
Tape 21: -2: [___0100011001_____111]
Tape 22: -2: [_____00001__00001]
Tape 23: -2: [___00001__1111__1111]
Tape 24: -2: [____0001___0001__11]
Tape 25: -2: [___1111_______100111101]
Tape 26: -2: [____0001]
Tape 27: -2: [___0100011001]
Tape 28: -2: []
Tape 29: -2: []
Tape 30: -2: []
Tape 31: 0: [00100011001]

141056 steps
halted ? true
Output: 


Return value:
610
```

## The Restricted C++
The restricted subset of C++, which I'll abbreviate as rC++, contains 
- `bool` data type
- `int` data type, but with arbitrary precision
- Arithmetical operators on ints: `+`,`-`,`*`,`/`,`%`,`^`,`-` for addition, subtraction, multiplication, floor division, modulo, exclusive-or, and the unary minus for negation. Operators `+=`, `-=`, `*=`, `/=`, `%=`, and `^=` for compound assignment on ints.
- Comparison operators on ints: `==`,`!=`,`<`,`<=`,`>`,`>=` for equals, not equals, less than, less than or equal to, greater than, and greater or equal to 
- Logical operators and comparison operators on bools: `==`,`!=`,`&&`,`||`,`^`,`!` for equals, not equals, AND, OR, exclusive-or, and the not operator. Operators `&=`, `|=`, `^=` for compound assignment on bools.
- Declaration and assignment of variables, like `int x = 999999999999999999999;` or `bool b = false;`
- For-loops, while-loops, break, continue, if statements
    - short-circuiting of boolean expressions
- Functions that can be recurisve or mutually recursive
- 3 provided functions:
    - `int nextInt()`, which reads in an integer from a tape designated as standard input
	- `void printInt(int x)` which prints an integer to a tape designated as standard output
    - `void printSpace()` which prints a space on the output tape. Should always be used after every call to `printInt`.
- A global int array `MEM` of theoretically infinite size, with values initialized to 0. You can get and set values in the array with statements like `MEM[3] = 5;` or `int x = MEM[3];`. `MEM` is not random-access.

Things that are NOT part of rC++ include, but are not limited to,
- floats or strings or any other types besides `int` and `bool`
    - you cannot have something like `int[] x = MEM;` either
- classes or structs or objects
- pointers or references
- preprocessor directives
- multiple files
- the operators `++`, `--`, `<<`, `>>`, `<<=`, `>>=`, `~`, `\&`, `|`
- casting
- global variables (though you can use a value in `MEM` to act as a global variable)
- function forward-declarations
- malloc/free/new/delete
- do-while loop
- use an int for a boolean expression

The number of tapes in the multi-tape Turing Machine varies per program, but it will be at least 30 tapes. Writing a function that takes a lot of parameters is one way to increase the number of tapes used. While each tape is two-way infinite, each tape in the generated multi-tape Turing Machines will never go lower than index `-2`, so they are more like one-way infinite tapes. There are three symbols in the alphabet used by the multi-tape Turing Machine: `0`, `1`, and `_` (blank). A transition in a multi-tape Turing Machine with T tapes is denoted using a regular expression that matches T characters read from the T tapes. This is used rather than specifying all possible values that the T tapes could read to reduce space. When following a transition, each tape can move left, right, or not move.

## Writing your own program
Create a new file in the `TMCompiler/programs/` directory using a text editor. The file name should end in the `.cpp` file extension. The program should contain valid C++ syntax. Reference the existing files in the directory for examples.

Make sure the program contains a main function in the format of `int main()`. Ensure this `main` function actually returns an int, such as `return 0;` at the end. 

You can use the provided functions `nextInt`, `printInt`, and `printSpace` for input and output. Always use `printSpace()` after every call to `printInt()`.

You can also use `MEM` as a global `int` array to store information. The cost of using bit	`y` of `MEM[x]` is proportional to `(x+y)*(x+y)`, so it is more expensive to request large indices than small indices. You can also write comments using line comments `//` as well as block comments `/* ... */`. Do not nest block comments.

When you are done writing the program, ensure that you did not accidentally use `++` or `--` for increments or decrements, and that the `main` function returns a value.

## Project Structure
- TMCompiler/TMCompiler/utils/ contains helper functions (for searching and transforming text) as well as json.hpp from nlohmann::json for serialization 
- TMCompiler/TMCompiler/tm\_definition/ defines classes to represent a tape, a transition, and a multi-tape Turing Machine
- TMCompiler/TMCompiler/compilation/
    - `unit2.hpp` exposes the function `souceToAssembly`
    - `unit1.hpp` exposes the function `assemblyToMultiTapeTuringMachine`
    - `compile.cpp` uses both functions to implement the `compile` function, which is exposed in `compile.hpp`
- TMCompiler/TMCompiler/runner/
    - `compileRunner.cpp` contains the `main` function that is run by the `compile` executable
    - `simulateRunner.cpp` contains the `main` function that is run by the `simulate` executable

## Extra details
Integers are written to the tapes in the following way: 
- the integer `0` is written as `0`
- otherwise, the first bit on the left is a sign bit: `1` for negative, `0` for non-negative
- the other bits, from left to right, go from least-significant bit to most-significant bit
- the only exception are the tapes reserved for storing instruction pointers: they are stored in the standard two's-complement form

The tapes are organized as follows:
- 1 for INPUT
- 1 for OUTPUT
- 1 for Instruction Pointer stack for keeping track of function calls
- 1 for Instrution Pointer
- some tapes for writing out the Instruction Pointer sideways
- 1 for Variable Stack for parameters
- 1 for index of head of tape of `MEM`
- 1 for	`MEM`
- some tapes for user variables
- 1 for return value

Accessing bit `y` of `MEM[x]` requires the head of the tape reserved for `MEM` to move to index `((x+y)(x+y+1) / 2) + x`.

## Things to work on
- Rework the Constants file  
- Clean up `unit1.cpp`
- Rework CMake  
- Optimizations  

## Ideas for Optimization  
- [x] Don't erase the value when popping off from a stack, leaving residual memory
- [x] Rework basic\_add, isPos, isNeg, isZero, and other functions implemented in the TM to be in-place, to avoid unnecessary pushes and pops  
- [ ] Incorporate "sideways" ip when changing ip. Should half the work of incrementing IP  

## Ideas for programs
- Implement some standard data structures and algorithms. It's a little bit harder when you only have an array!
- Write a Universal Turing Machine that takes in a description of a Turing Machine as well as input, and simulates the provided Turing Machine on the input.
- Pretend that MEM is all of a computer's memory, and designate some (infinite) section to be disc space, some section to be for RAM, and some section for a process's heap and stack. You could write a bootloader that runs when the Turing machine starts, and you could run an operating system on the Turing Machine.

## Future ideas 
- Add more features into what is allowed in the restricted C++, like strings and floats
- Convert from multi-tape Turing Machine into a "standard" single-tape Turing Machine of two symbols, with only left and right shifts. However, a quadratic slowdown may make it impractical to simulate.
    - Suppose I take all valid rC++ programs of at most 512 bytes, and compile them into standard single-tape Turing Machines. Let `Q` be the maximum number of states among all generated standard Turing machines, let `BB(n)` be the `n`th Busy Beaver Number, and let `Comp(n)` be the largest integer computed by any rC++ program of at most `n` bytes. Then we have `Comp(512) <= BB(Q)`. This means any rC++ entry to a Bignum Bakeoff Contest will compute some number that is at most `BB(Q)`. Equivalently, you could write a deterministic, halting rC++ program in at most 512 bytes that returns a large number, use some mathematics to prove that the program computes some explicit integer `N` (or prove that the number computed is at least as large as `N`), then claim that `N` is a lower bound for `BB(Q)`.
    - This project first converts rC++ to my own assembly code, then from assembly code to a multi-tape Turing Machine. If I can determine an upper bound on the number of lines of assembly generated by any rC++ program of at most 512 bytes, as well as determine the maximum number of times `newNode()` is called for any assembly instruction, then I can determine an upper bound on the maximum number of states in any multi-tape Turing Machine generated by this project from a rC++ program of at most 512 bytes. Then by figuring out how the number of states increases by changing it to a standard Turing Machine (probably increases it by some polynomial amount), then determining an explicit `Q` for which `Comp(512) <= BB(Q)` is true becomes possible.


Any comments, questions, or suggestions are welcome!
