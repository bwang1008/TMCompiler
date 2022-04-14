#ifndef MULTI_TAPE_TURING_MACHINE_HPP
#define MULTI_TAPE_TURING_MACHINE_HPP

#include <vector>				// std::vector

#include "TMCompiler/tm_definition/tape.hpp"
#include "TMCompiler/tm_definition/transition.hpp"

class MultiTapeTuringMachine {
	public:
		MultiTapeTuringMachine(const unsigned int numStates,
				const unsigned int numTapes,
				const unsigned int initialState,
				const unsigned int haltState,
				const std::vector<Transition> transitions);

		void setInput(const std::string &input);
		void setInput(const std::string &input, const int tapeIndex);
		bool halted() const;
		int step(const int verbose=0);

		// run, until reject/accept, return (ongoing/halted, number of steps)
		std::tuple<int, int> run(const int verbose=0, const int maxSteps=-1);

		void displayTape(const int tapeIndex) const;
		void displayTapes() const;
		void displayProfile() const;

	private:
		unsigned int Q;						// number of states, indexed from [0, Q-1]
		unsigned int T;						// number of tapes
		unsigned int initialState;
		unsigned int currentState;
		unsigned int haltState;

		std::vector<Tape> tapes;
		std::vector<std::vector<Transition> > transitions;

		Transition findTransition(const int state, 
				const std::vector<char> &symbols) const;
};

#endif
