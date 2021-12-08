#include "config.h"
#include "singleTapeTuringMachine.h"

#include <iostream>					// cin, cout, std::endl
#include <unordered_set>			// unordered_set
#include <unordered_map>			// unordered_map
#include <tuple>					// tuple
#include <string>					// string
   
SingleTapeTuringMachine::SingleTapeTuringMachine(int numStates, int initialState, std::unordered_set<int> accept, std::vector<std::tuple<  std::tuple<int, int> , std::tuple<int, int, int> >> transitionsVec, const std::string& input, int offset): Q{numStates}, initialState{initialState}, currentState{initialState} {
	tape = new Tape(input, offset);
	acceptStates = accept;

	for(std::vector<std::tuple<std::tuple<int, int>, std::tuple<int, int, int>>>::iterator it = transitionsVec.begin(); it != transitionsVec.end(); ++it) {
		std::tuple<int, int> left = std::get<0>(*it);
		std::tuple<int, int, int> right = std::get<1>(*it);

		int state = std::get<0>(left);
		int symbol = std::get<1>(left);

		int state2 = std::get<0>(right);
		int symbol2 = std::get<1>(right);
		int shift = std::get<2>(right);

		long long key = ((long long) symbol) * Q + state;
		long long val = ((long long) symbol2) * Q + state2;
		val = 3 * val + shift;

		transitions[key] = val;
	}
}

std::vector<int> SingleTapeTuringMachine::findTransition(int state, int symbol) {
	long long key = ((long long) symbol) * Q + state;

	if(!transitions.count(key)) {
		std::vector<int> ret = {-1};
		return ret;
	}

	long long val = transitions[key];
	
	int shift = int(val % 3);
	val /= 3;

	int state2 = int(val % Q);
	val /= Q;
	
	int symbol2 = int(val);

	std::vector<int> ret{state2, symbol2, shift};
	return ret;
}

int SingleTapeTuringMachine::step(int verbose) {

	if(verbose == 2) {
		std::cout << "Head at position " << tape -> headPosition() << std::endl;
	}
	
	// check if already in ACCEPT state
	if(acceptStates.find(currentState) != acceptStates.end()) {
		if(verbose == 2) {
			std::cout << "Accepted state " << currentState << std::endl;
		}
		
		return Config::StateStatus::accept;
	}

	// check if there does not exist a move; if so, auto reject
	int symbol = tape->read();
	std::vector<int> result = findTransition(currentState, symbol);
	
	if(result[0] == -1) {
		if(verbose == 2) {
			std::cout << "Rejected state " << currentState << std::endl;
		}
		return Config::StateStatus::reject;
	}
	
	// otherwise, do transition:
	int state2 = result[0];
	int symbol2 = result[1];
	int shift = result[2];

	if(verbose == 2) {
		std::cout << "Transition from state " << currentState << ", sym " << symbol << " -> " << state2 << ", " << symbol2 << ", " << shift << std::endl;
		tape->display();
	}
	

	currentState = state2;
	tape->write(symbol2);
	
	tape->moveHead(shift);
	
	if(verbose == 2) {
		if(shift == Config::Shift::right) {
			std::cout << "Shifted head right" << std::endl;
		}
		if(shift == Config::Shift::left) {
			std::cout << "Shifted head left" << std::endl;
		}

	}

	
	return Config::StateStatus::ongoing;	
}

std::vector<int> SingleTapeTuringMachine::run(int verbose, int maxSteps) {
	int numSteps = 0;
	int status = step(verbose);

	while(status == Config::StateStatus::ongoing && (maxSteps < 0 || numSteps < maxSteps)) {
		numSteps++;
		status = step(verbose);
	}

	std::vector<int> ret{status, numSteps};
	return ret;
}

void SingleTapeTuringMachine::displayTape() {
	tape->display();
}
