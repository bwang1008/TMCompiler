#include "unit1.h"
#include "../tm_definition/transition.h"
#include "../tm_definition/multi_tape_turing_machine.h"

MultiTapeTuringMachine assemblyToMultiTapeTuringMachine(std::vector<std::string> &program) {
	std::vector<Transition> transitions;
	transitions.emplace_back(1, "what", 1, "lmao", std::vector<int> {0, 0, 0});
	MultiTapeTuringMachine mttm(3, 5, 0, 1, transitions);
	return mttm;
}
