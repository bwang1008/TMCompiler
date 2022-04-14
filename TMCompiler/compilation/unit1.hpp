#ifndef UNIT1_HPP
#define UNIT1_HPP

#include <string>		// std::string
#include <vector>		// std::vector

#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(const std::vector<std::string> &program);

#endif
