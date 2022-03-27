#ifndef MULTI_TAPE_TURING_MACHINE_H
#define MULTI_TAPE_TURING_MACHINE_H

#include "tape.h"
#include "transition.h"
#include "turing_machine.h"

#include <vector>				// std::vector

class MultiTapeTuringMachine : public TuringMachine {
public:
	MultiTapeTuringMachine(const int numStates, const int numTapes, const int initialState, const int haltState, const std::vector<Transition> transitions);
	~MultiTapeTuringMachine();

	void setInput(const std::string &input);
	void setInput(const std::string &input, const int tapeIndex);
	bool halted() const;
	int step(const int verbose=0);
	std::tuple<int, int> run(const int verbose=0, const int maxSteps=-1);		// run, until reject/accept, return (ongoing/halted, number of steps)

	void displayTape(const int tapeIndex) const;
	void displayTapes() const;

private:
	int Q;						// number of states, indexed from [0, Q-1]
	int T;						// number of tapes
	std::vector<Tape> tapes;
	int initialState;
	int currentState;
	int haltState;

	std::vector<Transition> transitions;

	Transition findTransition(const int state, const std::vector<char> &symbols) const;
	
};

#endif
