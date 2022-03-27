#include "constants.h"
#include "tape.h"
#include "transition.h"

#include "single_tape_turing_machine.h"
#include "multi_tape_turing_machine.h"

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
	transitions.emplace_back(0, "[01]", 0, ".", std::vector<int>{Constants::Shift::right});
	transitions.emplace_back(0, "_", 1, ".", std::vector<int>{Constants::Shift::left});	
	transitions.emplace_back(1, "1", 1, "0", std::vector<int>{Constants::Shift::left});
	transitions.emplace_back(1, "[0_]", 2, "1", std::vector<int>{Constants::Shift::left});

	int maxSteps = -1;
	std::string input = "10010111";

	SingleTapeTuringMachine tm(numStates, initialState, haltState, transitions);
	tm.setInput(input);

	std::tuple<int, int> ans = tm.run(2, maxSteps);

	tm.displayTape();

	std::cout << "Status: " << std::get<0>(ans) << "; steps = " << std::get<1>(ans) << std::endl;
}

void testMTTM() {
	// addition of two ints (have to be of the same size), with overflow 
	
	int numStates = 4;
	int numTapes = 3;
	int initialState = 0;
	int haltState = numStates - 1;

	// state 0 is where ur heading right until reach the right end of the input
	// state 1 is where you have no carry, and you do the addition and head left
	// state 2 is where you have a carry, and you do the addition and head left
	// state 3 is the halt state

	std::vector<Transition> transitions;
	transitions.emplace_back(0, "[01][01].", 0, "...", std::vector<int>{Constants::Shift::right, Constants::Shift::right, Constants::Shift::right});
	transitions.emplace_back(0, "__.", 1, "...", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	
	transitions.emplace_back(1, "00.", 1, "000", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	transitions.emplace_back(1, "01.", 1, "011", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	transitions.emplace_back(1, "10.", 1, "101", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	transitions.emplace_back(1, "11.", 2, "110", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});

	transitions.emplace_back(1, "__.", 3, "___", std::vector<int>{Constants::Shift::right, Constants::Shift::right, Constants::Shift::right});

	transitions.emplace_back(2, "00.", 1, "001", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	transitions.emplace_back(2, "01.", 2, "010", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	transitions.emplace_back(2, "10.", 2, "100", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});
	transitions.emplace_back(2, "11.", 2, "111", std::vector<int>{Constants::Shift::left, Constants::Shift::left, Constants::Shift::left});

	transitions.emplace_back(2, "__.", 3, "___", std::vector<int>{Constants::Shift::right, Constants::Shift::right, Constants::Shift::right});

	int maxSteps = 20;
	//std::string input1("100111");
	std::string input2("010111");
	
	MultiTapeTuringMachine tm(numStates, numTapes, initialState, haltState, transitions);
	
	tm.setInput("100111", 0);
	tm.setInput(input2, 1);

	std::cout << "before running:" << std::endl;
	tm.displayTapes();
	std::cout << "now run:" << std::endl;


	std::tuple<int, int> ans = tm.run(1, maxSteps);

	tm.displayTapes();

	std::cout << "Status: " << std::get<0>(ans) << "; steps = " << std::get<1>(ans) << std::endl;

}

int main() {
	testMTTM();
}
