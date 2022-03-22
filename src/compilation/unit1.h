#ifndef UNIT1_H
#define UNIT1_H

#include "../tm_definition/multi_tape_turing_machine.h"

#include <iostream>
#include <vector>

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(std::vector<std::string> &program);

#endif
