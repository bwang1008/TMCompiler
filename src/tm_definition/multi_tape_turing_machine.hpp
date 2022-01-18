#ifndef MULTI_TAPE_TURING_MACHINE_H
#define MULTI_TAPE_TURING_MACHINE_H

#include "tape.hpp"
#include "transition.hpp"
#include "turing_machine.hpp"

#include <vector>				// std::vector

class MultiTapeTuringMachine : public TuringMachine {
public:
	MultiTapeTuringMachine(int numStates, int numTapes, int initialState, int haltState, std::vector<Transition> transitions);
	~MultiTapeTuringMachine();

	void setInput(const std::string &input);
	bool halted() const;
	int step(const int verbose=0);
	std::tuple<int, int> run(const int verbose=0, const int maxSteps=-1);		// run, until reject/accept, return (ongoing/halted, number of steps)

	void displayTape(const int tapeIndex):

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
