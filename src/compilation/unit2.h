#ifndef UNIT2_H
#define UNIT2_H

#include <string>
#include <vector>

/**
 * Mid refers to source code, with data types of arbitrary-precision integer, and boolean. There is 
 * one global integer array. 
 *
 * Low refers to assembly-like code, with no ifs/loops, but jumps and reading/writing memory
 * 
 * Takes in a program (vector of strings), does some transformations / translations on it, and returns new one
 * If error occurs, return program (vector of strings) of size 0 instead
 */
std::vector<std::string> sourceToAssembly(std::vector<std::string> &program);

#endif
