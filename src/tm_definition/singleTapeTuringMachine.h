#ifndef SINGLETAPEURINGMACHINE_H
#define SINGLETAPETURINGMACHINE_H

#include "tape.h"

#include <string>					// string
#include <tuple>					// tuple, make_tuple
#include <unordered_map>			// unordered_map
#include <unordered_set>			// unordered_set
#include <vector>					// vector

class SingleTapeTuringMachine {
	Tape* tape;
	int Q;	// number of states, indexed from [0, Q-1]
	int initialState;
	int currentState;
	
	std::unordered_set<int> acceptStates;
	std::unordered_map<long long, long long> transitions;

	std::vector<int> findTransition(int state, int symbol);		// helper to use transitions map (cuz map of tuples is weird)

public:
	SingleTapeTuringMachine(int numStates, int initialState, std::unordered_set<int> accept, std::vector<std::tuple<  std::tuple<int, int> , std::tuple<int, int, int> >> transitionsVec, const std::string& input, int offset);			// constructor
	
	int step(int verbose=0);		// make a single transition step
	std::vector<int> run(int verbose=0, int maxSteps=-1);		// run, until reject/accept. return (accept=1/reject=0, number of steps)

	void displayTape();
};



#endif
