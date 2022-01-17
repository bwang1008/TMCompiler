#ifndef TRANSITION_H
#define TRANSITION_H

#include <string>
#include <vector>

// suppose we have T tapes
// transition from (state1, T different symbols)
// -> (state2, T different symbols, T different shifts)
// each symbol represented by one character in regex
struct Transition {
	int state1;
	std::string symbols1;
	int state2;
	std::string symbols2;
	std::vector<int> shifts;

	Transition(int state1, std::string &&symbols1, int state2, std::string &&symbols2, std::vector<int> &&shifts);
	
	Transition(int state1, const char symbols1[], int state2, const char symbols2[], std::vector<int> &&shifts);
	
	bool operator< (const Transition other) const;
};

#endif
