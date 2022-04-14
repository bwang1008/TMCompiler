#ifndef COMPILE_HPP
#define COMPILE_HPP

#include <string>		// std::string

#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"

// takes in a file for C++-like code and turns into a MultiTapeTuringMachine

MultiTapeTuringMachine compile(const std::string &fileName);

#endif
