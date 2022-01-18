#ifndef SINGLE_TAPE_TURING_MACHINE_H
#define SINGLE_TAPE_TURING_MACHINE_H

#include "tape.hpp"
#include "transition.hpp"
#include "turing_machine.hpp"

#include <string>					// std::string
#include <vector>					// std::vector

class SingleTapeTuringMachine : public TuringMachine {
public:
	SingleTapeTuringMachine(const int numStates, const int initialState, const int haltState, const std::vector<Transition> transitions);

	~SingleTapeTuringMachine();

	void setInput(const std::string &input);
	bool halted() const;
	int step(const int verbose=0);		// make a single transition step
	std::tuple<int, int> run(const int verbose=0, const int maxSteps=-1);		// run, until reject/accept. return (ongoing/halted, number of steps)

	void displayTape() const;

private:
	Tape tape;
	int Q;							// number of states, indexed from [0, Q-1]
	int initialState;
	int currentState;
	int haltState;

	std::vector<Transition> transitions;

	Transition findTransition(const int state, const char symbol) const; 
};

#endif
