# TMCompiler
Convert a very restricted subset of C++ code into a multi-tape Turing Machine representation

High-level source code is a restricted-subset of C++: that of int and bool datatypes (but int is arbitrary precision),
functions, common operators like +,-,*,/,%,<,>,<=,>=,==,!=,!,||,&&, and ^. There is also a global integer array named 
MEM, starting at index 0. No global variables are allowed besides the already defined MEM. There are also 3 global 
functions to use: nextInt, printInt, and printSpace. nextInt reads in one integer at a time from an input tape that is
blank-separated. printInt and printSpace operate on an output tape.

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

Next ideas:
Finish implementing special functions.
Should rewrite Turing Machine definition (especially Transitions)
Should rework Constants file
Should rework CMake

Future ideas:
Add more features into what is allowed in the restricted C++, like strings and floats.
Convert from multi-tape Turing Machine into a single-tape Turing Machine with only left and right shifts

