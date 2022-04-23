#ifndef MULTI_TAPE_TURING_MACHINE_HPP
#define MULTI_TAPE_TURING_MACHINE_HPP

#include <string>				// std::string
#include <vector>				// std::vector

#include "TMCompiler/tm_definition/tape.hpp"
#include "TMCompiler/tm_definition/transition.hpp"
#include "TMCompiler/utils/json.hpp"

class MultiTapeTuringMachine {
	public:
		MultiTapeTuringMachine();
		MultiTapeTuringMachine(const unsigned int numStates,
				const unsigned int numTapes,
				const unsigned int initialState,
				const unsigned int haltState,
				const std::vector<Transition> transitions);

		void setInput(const std::string &input);
		void setInput(const std::string &input, const int tapeIndex);
		bool halted() const;
		unsigned int numTapes() const;
		unsigned int numSteps() const;
		void step(const int verbose=0);

		// run, until reject/accept, or maxSteps
		void run(const int verbose=0, const int maxSteps=0);

		void displayTape(const int tapeIndex) const;
		void displayTapes() const;
		void displayProfile() const;
		std::string tapeContents(const int tapeIndex) const;

		// allow these non-member functions to access all members for json serialization
		friend void to_json(nlohmann::json &j, const MultiTapeTuringMachine &mttm);
		friend void from_json(const nlohmann::json &j, MultiTapeTuringMachine &mttm);

	private:
		unsigned int Q;						// number of states, indexed from [0, Q-1]
		unsigned int T;						// number of tapes
		unsigned int initialState;
		unsigned int currentState;
		unsigned int haltState;
		unsigned int steps;

		std::vector<Tape> tapes;
		std::vector<std::vector<Transition> > transitions;

		Transition findTransition(const int state, 
				const std::vector<char> &symbols) const;
};

#endif
