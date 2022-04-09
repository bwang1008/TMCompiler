#ifndef UNIT1_H
#define UNIT1_H

#include <iostream>
#include <vector>

#include "../tm_definition/multi_tape_turing_machine.h"

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(const std::vector<std::string> &program);

#endif
