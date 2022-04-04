# TMCompiler
Attempt to convert C-like code into a Turing Machine representation

Plan Ideas:

High-level source code will have an arbitrary-precision integer data-type, as well as boolean data-type.
There will be one global integer array to use. Accessing integer array x, bit y, 
actually accesses underlying-char array (x+y)(x+y) + y, borrowed from 
https://cstheory.stackexchange.com/questions/18688/a-small-c-like-language-that-turing-machines-can-simulate.  

Integer will be in little-endian: least-significant-bit on the left, to be able to do integer arithmetic.

Source code will be compiled to a multi-tape Turing Machine, before being converted to a single-tape Turing Machine.

Generated Turing Machine will have either 3 symbols: 0, 1, and space ( _ )

Number of tapes:
1 for INPUT
1 for OUTPUT
1 for Instruction Pointer stack
1 for Instrution Pointer
num bits in Instruction Pointer
1 for User Memory: Integer Array
1 for Index of User Memory
1 for Variable Stack
V for each declared variable, where V = max number of declared variables (including temp variables and parameters) across all functions
1 for RETURN


Next ideas:
Finish implementing special functions.
Should rewrite Turing Machine definition (especially Transitions)
Should rework Constants file
Should rework CMake

