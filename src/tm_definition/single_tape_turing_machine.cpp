#include "constants.hpp"
#include "single_tape_turing_machine.hpp"

#include <algorithm>	// std::sort
#include <iostream>		// std::cerr, std::cout, std::endl
#include <tuple>		// std::tuple, std::make_tuple
#include <regex>		// std::regex, std::regex_match

SingleTapeTuringMachine::SingleTapeTuringMachine(const int numStates, const int initialState, const int haltState, const std::vector<Transition> transitions) : tape("", 0), Q(numStates), initialState(initialState), currentState(initialState), haltState(haltState) {
	//this->tape = new Tape("", 0);
	this->transitions = transitions;
	std::sort(this->transitions.begin(), this->transitions.end());
}

SingleTapeTuringMachine::~SingleTapeTuringMachine() {}

void SingleTapeTuringMachine::setInput(const std::string &input) {
	this->tape.clear();
	for(size_t i = 0; i < input.size(); ++i) {
		char c = input[i];
		this->tape.write(c);
		this->tape.moveHead(Constants::Shift::right);
	}

	for(size_t i = 0; i < input.size(); ++i) {
		this->tape.moveHead(Constants::Shift::left);
	}
}

bool SingleTapeTuringMachine::halted() const {
	return this->currentState == this->haltState;
}

Transition SingleTapeTuringMachine::findTransition(const int state, const char symbol) const {
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

	std::string input(1, symbol);
	for(int i = low; i < (int) this->transitions.size() && this->transitions[i].state1 == state; ++i) {
		Transition temp = this->transitions[i];
		if(std::regex_match(input, std::regex(temp.symbols1))) {
			return temp;
		}
	}

	// could not find it
	Transition temp(-1, ".", -1, ".", std::vector<int>{0});

	return temp;
}

int SingleTapeTuringMachine::step(const int verbose) {
	// check if already in HALT state

	if(this->halted()) {
		if(verbose >= 2) {
			std::cout << "Already reached HALT state " << this->haltState << std::endl;
		}

		return Constants::StateStatus::halted;
	}

	char symbol = this->tape.read();
	Transition transition = this->findTransition(this->currentState, symbol);

	// if no valid transition found,
	if(transition.state1 == -1) {
		// implied that halting state reached
		Transition temp(this->currentState, std::string(1, symbol), this->haltState, std::string(1, symbol), std::vector<int>{Constants::Shift::none});
		
		transition = temp;

		if(verbose >= 1) {
			std::cerr << "Could not find transition for " << transition.state1 << ", " << symbol << std::endl;
		}
	}

	int state2 = transition.state2;
	char symbol2 = transition.symbols2[transition.symbols2.size() - 1];

	// if symbols2 == '.', means new char that is written == old char that was read
	symbol2 = (symbol2 == '.' && transition.symbols2.size() == 1) ? symbol : symbol2;

	int shift = transition.shifts[0];

	if(verbose >= 1) {
		std::cout << "Transition from (q" << this->currentState << ", " << symbol << ") -> (q" << state2 << ", " << symbol2 << ", " << shift << ")" << std::endl;
	}

	this->currentState = state2;
	this->tape.write(symbol2);
	this->tape.moveHead(shift);

	return (this->halted()) ? Constants::StateStatus::halted : Constants::StateStatus::ongoing;	
}

std::tuple<int, int> SingleTapeTuringMachine::run(const int verbose, const int maxSteps) {
	int numSteps = 0;

	while(!this->halted() && (maxSteps < 0 || numSteps < maxSteps)) {
		++numSteps;
		this->step(verbose);
	}

	int status = (this->halted()) ? Constants::StateStatus::halted : Constants::StateStatus::ongoing;
	std::tuple<int, int> ret = std::make_tuple(status, numSteps);
	
	return ret;
}

void SingleTapeTuringMachine::displayTape() const {
	this->tape.display();	
}
