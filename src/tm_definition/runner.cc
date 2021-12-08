#include "config.h"
#include "tape.h"
#include "singleTapeTuringMachine.h"

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
	while(c != Config::blank && repetition < maxRepetitions) {
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

	std::unordered_set<int> accept;
	accept.insert(2);

	std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int, int>>> transitions;
	transitions.push_back(std::make_tuple(std::make_tuple(0, Config::zero), std::make_tuple(0, Config::zero, Config::Shift::right)));

	transitions.push_back(std::make_tuple(std::make_tuple(0, Config::one), std::make_tuple(0, Config::one, Config::Shift::right)));
		
	transitions.push_back(std::make_tuple(std::make_tuple(0, Config::blank), std::make_tuple(1, Config::blank, Config::Shift::left)));

	transitions.push_back(std::make_tuple(std::make_tuple(1, Config::one), std::make_tuple(1, Config::zero, Config::Shift::left)));

	transitions.push_back(std::make_tuple(std::make_tuple(1, Config::zero), std::make_tuple(2, Config::one, Config::Shift::left)));

	transitions.push_back(std::make_tuple(std::make_tuple(1, Config::blank), std::make_tuple(2, Config::one, Config::Shift::left)));

	int maxSteps = -1;

	const std::string input = "11111111111111";
	int offset = 0;

	SingleTapeTuringMachine* tm = new SingleTapeTuringMachine(numStates, initialState, accept, transitions, input, offset);

	std::vector<int> ret = tm->run(0, maxSteps);

	tm->displayTape();

	std::cout << "Status: " << ret[0] << "; steps = " << ret[1] << std::endl;
}

int main() {
	testSTTM();	
}
