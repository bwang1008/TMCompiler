#include "multi_tape_turing_machine.hpp"

#include <algorithm>		// std::sort
#include <iostream>			// std::cout, std::endl
#include <map>				// std::map
#include <regex>			// std::regex, std::regex_match
#include <string>			// std::string
#include <tuple>			// std::tuple, std::make_tuple
#include <vector>			// std::vector

#include "TMCompiler/utils/constants.hpp"

MultiTapeTuringMachine::MultiTapeTuringMachine(const unsigned int numStates,
		const unsigned int numTapes,
		const unsigned int initialState,
		const unsigned int haltState,
		const std::vector<Transition> transitions) :
Q{numStates},
T{numTapes},
tapes(numTapes, Tape("")),
initialState{initialState},
currentState{initialState},
haltState{haltState} {
	for(const Transition transition : transitions) {
		const unsigned int state1 = transition.state1;
		this->transitions[state1].push_back(transition);
	}

	// sort transitions for the one node with many transitions: node "after" : 3
	for(size_t i = 0; i < this.transitions.size(); ++i) {
		if(this.transitions[i].size() > 15) {
			std::sort(this.transitions[i].begin(),
					this.transitions[i].end(),
					[](const Transition &t1, const Transition &t2) -> bool {
						t1.compare(t2) < 0;
					}
			);
		}
	}
}

void MultiTapeTuringMachine::setInput(const std::string &input) {
	this->setInput(input, 0);
}

void MultiTapeTuringMachine::setInput(const std::string &input, const int tapeIndex) {
	// tape tapeIndex is the INPUT tape
	this->tapes[tapeIndex].clear();
	for(size_t i = 0; i < input.size(); ++i) {
		char c = input[i];
		this->tapes[tapeIndex].write(c);
		this->tapes[tapeIndex].moveHead(Constants::Shift::right);
	}

	for(size_t i = 0; i < input.size(); ++i) {
		this->tapes[tapeIndex].moveHead(Constants::Shift::left);
	}
}

bool MultiTapeTuringMachine::halted() const {
	return this->currentState == this->haltState;
}

Transition MultiTapeTuringMachine::findTransition(const int state, const std::vector<char> &symbols) const {
	int low = 0;
	int high = (int) (this->transitions.size() - 1);

	while(low <= high) {
		int mid = (low + high) / 2;
		if(this->transitions[mid].state1 < state) {
			low = mid + 1;
		}
		else {
			high = mid - 1;
		}
	}

	std::string input(symbols.begin(), symbols.end());
	for(int i = low; i < (int) this->transitions.size() && this->transitions[i].state1 == state; ++i) {
		Transition temp = this->transitions[i];
		if(std::regex_match(input, std::regex(temp.symbols1))) {
			return temp;
		}
	}

	// could not find it
	Transition temp(-1, ".", -1, ".", std::vector<int>(this->T, Constants::Shift::none));

	return temp;
}

int MultiTapeTuringMachine::step(const int verbose) {
	// check if already in HALT state

	if(this->halted()) {
		if(verbose >= 2) {
			std::cout << "Already reached HALT state " << this->haltState << std::endl;
		}

		return Constants::StateStatus::halted;
	}

	std::vector<char> symbols;
	for(int t = 0; t < this->T; ++t) {
		char symbol = this->tapes[t].read();
		symbols.push_back(symbol);
	}

	Transition transition = this->findTransition(this->currentState, symbols);

	// if no valid transition found,
	if(transition.state1 == -1) {
		// implied that halting state reached
		std::string symbols2(symbols.begin(), symbols.end());
		Transition temp(this->currentState, std::string(symbols.begin(), symbols.end()), this->haltState, std::string(symbols.begin(), symbols.end()), std::vector<int>(this->T, Constants::Shift::none));
		
		transition = temp;

		if(verbose >= 1) {
			std::cerr << "Could not find transition for " << transition.state1 << ", " << std::string(symbols.begin(), symbols.end()) << std::endl;
		}
	}

	int state2 = transition.state2;
	std::string symbols2 = transition.symbols2;
	std::vector<int> shifts = transition.shifts;

	if(verbose >= 1) {
		std::cout << "Transition from (q" << this->currentState << ", [";
		
		for(int t = 0; t < this->T; ++t) {
			std::cout << symbols[t];
			if(t < this->T - 1) {
				std::cout << ", ";
			}
		}
		
		std::cout << "]) -> (q" << state2 << ", " << symbols2 << ", [";
		
		for(int t = 0; t < this->T; ++t) {
			std::cout << shifts[t];
			if(t < this->T - 1) {
				std::cout << ", ";
			}
		}

		std::cout << "])" << std::endl;
	}

	this->currentState = state2;
	
	size_t symbolStart = 0;
	for(int t = 0; t < this->T; ++t) {
		// if symbols2 == '.', means new char that is written == old char that was read
		
		char symbol = symbols2[symbolStart];
		if(symbols2[symbolStart] == '\\' && symbols2[symbolStart + 1] == '.') {
			symbol = '.';
			symbolStart += 2;
		}
		else {
			if(symbol == '.') {
				symbol = symbols[t];
			}
			symbolStart += 1;
		}
	
		this->tapes[t].write(symbol);

		int shift = shifts[t];
		this->tapes[t].moveHead(shift);
	}
	
	return (this->halted()) ? Constants::StateStatus::halted : Constants::StateStatus::ongoing;	
}

std::tuple<int, int> MultiTapeTuringMachine::run(const int verbose, const int maxSteps) {
	int numSteps = 0;

	while(!this->halted() && (maxSteps < 0 || numSteps < maxSteps)) {
		++numSteps;
		this->step(verbose);
	}

	int status = (this->halted()) ? Constants::StateStatus::halted : Constants::StateStatus::ongoing;
	std::tuple<int, int> ret = std::make_tuple(status, numSteps);
	
	return ret;
}

void MultiTapeTuringMachine::displayTape(const int tapeIndex) const {
	this->tapes[tapeIndex].display();
}

void MultiTapeTuringMachine::displayTapes() const {
	for(int t = 0; t < this->T; ++t) {
		std::cout << "Tape " << t << ": ";
		this->displayTape(t);
	}
}

void MultiTapeTuringMachine::displayProfile() const {
	std::cout << this->Q << " states" << std::endl;
	std::cout << this->T << " tapes" << std::endl;
	std::cout << this->transitions.size() << " transitions" << std::endl;
	std::cout << "Start at node " << this->initialState << " and ends at " << this->haltState << std::endl;

	// map from nodes, to number of transitions out
	std::map<int, int> counts;
	for(std::vector<Transition>::const_iterator it = this->transitions.cbegin(); it != this->transitions.cend(); ++it) {
		int node = it->state1;
		counts[node] += 1;
	}

	// count number of nodes have this many transitions
	std::map<int, int> counts2;
	for(std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
		int b = it->second;
		counts2[b] += 1;
	}

	int lowerLimit = 0;
	for(std::map<int, int>::iterator it = counts2.begin(); it != counts2.end(); ++it) {
		if(it->first >= lowerLimit) {
			std::cout << it->second << " nodes have " << it->first << " transitions" << std::endl;
		}
	}
}
