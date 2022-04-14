#include "multi_tape_turing_machine.hpp"

#include <algorithm>		// std::sort
#include <iostream>			// std::cout, std::endl
#include <map>				// std::map
#include <regex>			// std::regex, std::regex_match
#include <string>			// std::string
#include <tuple>			// std::tuple, std::make_tuple
#include <vector>			// std::vector

#include "TMCompiler/utils/constants.hpp"
#include "TMCompiler/tm_definition/tape.hpp"
#include "TMCompiler/tm_definition/transition.hpp"

MultiTapeTuringMachine::MultiTapeTuringMachine(const unsigned int numStates,
		const unsigned int numTapes,
		const unsigned int initialState,
		const unsigned int haltState,
		const std::vector<Transition> transitions) :
Q{numStates},
T{numTapes},
initialState{initialState},
currentState{initialState},
haltState{haltState},
tapes(numTapes, Tape("")) {
	for(size_t i = 0; i < this->Q; ++i) {
		this->transitions.push_back(std::vector<Transition>());
	}

	for(const Transition transition : transitions) {
		const unsigned int state1 = transition.state1;
		this->transitions[state1].push_back(transition);
	}

	// sort transitions for the one node with many transitions: node "after" : 3
	for(size_t i = 0; i < this->transitions.size(); ++i) {
		if(this->transitions[i].size() > 15) {
			std::sort(this->transitions[i].begin(),
					this->transitions[i].end(),
					[](const Transition &t1, const Transition &t2) -> bool {
					return t1.symbols1.compare(t2.symbols1) < 0;
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
	const std::vector<Transition> validTransitions = this->transitions[state];
	const std::string input(symbols.begin(), symbols.end());
	
	// special case: node == "after": many transitions, but are sorted. Use binary search to find write one
	if(state == 3) {
		int low = 0;
		int high = static_cast<int>(this->transitions.size() - 1);

		while(low <= high) {
			const int mid = (low + high) / 2;
			const Transition temp = validTransitions[mid];
			if(std::regex_match(input, std::regex(temp.symbols1))) {
				return validTransitions[mid];
			}
			else {
				std::string readRule;
				for(size_t i = 0; i < temp.symbols1.size(); ++i) {
					if(temp.symbols1[i] == '.') {
						readRule.push_back(input[i]);
					}
					else {
						readRule.push_back(temp.symbols1[i]);
					}
				}

				if(input.compare(readRule) < 0) {
					high = low - 1;
				}
				else {
					low = high + 1;
				}
			}
		}

		// could not find it
		const Transition temp(this->Q + 1, std::string('.', this->T), this->Q + 1, std::string('.', this->T), std::vector<int>(this->T, Constants::Shift::none));
		return temp;
	}
	else {
		// other nodes only have at most 10 transitions; do a linear search
		for(size_t i = 0; i < validTransitions.size(); ++i) {
			Transition temp = validTransitions[i];
			if(std::regex_match(input, std::regex(temp.symbols1))) {
				return temp;
			}
		}

	}

	// could not find it
	const Transition temp(this->Q + 1, std::string('.', this->T), this->Q + 1, std::string('.', this->T), std::vector<int>(this->T, Constants::Shift::none));
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
	for(size_t t = 0; t < this->T; ++t) {
		char symbol = this->tapes[t].read();
		symbols.push_back(symbol);
	}

	Transition transition = this->findTransition(this->currentState, symbols);

	// if no valid transition found,
	if(transition.state1 == this->Q + 1) {
		// implied that halting state reached
		const std::string symbols2(symbols.begin(), symbols.end());
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
		
		for(size_t t = 0; t < this->T; ++t) {
			std::cout << symbols[t];
			if(t < this->T - 1) {
				std::cout << ", ";
			}
		}
		
		std::cout << "]) -> (q" << state2 << ", " << symbols2 << ", [";
		
		for(size_t t = 0; t < this->T; ++t) {
			std::cout << shifts[t];
			if(t < this->T - 1) {
				std::cout << ", ";
			}
		}

		std::cout << "])" << std::endl;
	}

	this->currentState = state2;
	
	size_t symbolStart = 0;
	for(size_t t = 0; t < this->T; ++t) {
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
	for(size_t t = 0; t < this->T; ++t) {
		std::cout << "Tape " << t << ": ";
		this->displayTape(t);
	}
}

void MultiTapeTuringMachine::displayProfile() const {
	std::cout << this->Q << " states" << std::endl;
	std::cout << this->T << " tapes" << std::endl;
	std::cout << "Start at node " << this->initialState << " and ends at " << this->haltState << std::endl;

	// count number of nodes have this many transitions
	std::map<int, int> counts;
	int numTransitions = 0;
	for(size_t i = 0; i < this->transitions.size(); ++i) {
		int b = static_cast<int>(this->transitions[i].size());
		numTransitions += b;
		counts[b] += 1;
	}

	std::cout << numTransitions << " transitions" << std::endl;

	for(std::map<int, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
		std::cout << it->second << " nodes have " << it->first << " transitions" << std::endl;
	}
}
