#include "constants.hpp"
#include "tape.hpp"
#include "transition.hpp"
#include "single_tape_turing_machine.hpp"

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

void testTape() {
	Tape* tape = new Tape("abacd", -3);
	
	int maxRepetitions = 20;
	int repetition = 0;
	
	int c = tape->read();
	while(c != Constants::blank && repetition < maxRepetitions) {
		std::cout << "read " << char(c) << std::endl;
		tape->moveHeadRight();
		c = tape->read();

		repetition++;
	}

	tape->display();	
}

void testSTTM() {
	// binary increment, taken from turingmachine.io
	
	int numStates = 3;
	int initialState = 0;
	int haltState = 2;
	
	std::vector<Transition> transitions;
	transitions.emplace_back(0, "0", 0, "0", std::vector<int>{Constants::Shift::right});
	transitions.emplace_back(0, "1", 0, "1", std::vector<int>{Constants::Shift::right});
	transitions.emplace_back(0, "_", 1, "_", std::vector<int>{Constants::Shift::left});

	transitions.emplace_back(1, "1", 1, "0", std::vector<int>{Constants::Shift::left});
	transitions.emplace_back(1, "[0_]", 2, "1", std::vector<int>{Constants::Shift::left});

	int maxSteps = -1;

	std::string input = "10010111";

	SingleTapeTuringMachine tm(numStates, initialState, haltState, transitions);
	
	tm.setInput(input);

	std::tuple<int, int> ans = tm.run(0, maxSteps);

	tm.displayTape();

	std::cout << "Status: " << std::get<0>(ans) << "; steps = " << std::get<1>(ans) << std::endl;
}


int main() {
	testSTTM();
}
