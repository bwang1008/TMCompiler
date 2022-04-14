#ifndef UNIT2_HPP
#define UNIT2_HPP

#include <string>		// std::string
#include <vector>		// std::vector

/**
 * "program" refers to C++-like source code, with data types of arbitrary-precision integer, and boolean. 
 * There is one global integer array. 
 *
 * Returns assembly-like code, with assignments, function calls, push, pop, and jump-if-false
 * 
 * Takes in a program (vector of strings), does some transformations / translations on it, and returns new one
 */
std::vector<std::string> sourceToAssembly(const std::vector<std::string> &program);

#endif
