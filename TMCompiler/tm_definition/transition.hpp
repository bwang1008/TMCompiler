#ifndef TRANSITION_HPP
#define TRANSITION_HPP

#include <string>		// std::string
#include <vector>		// std::vector

/**
 * suppose we have T tapes
 * transition from (state1, T different symbols)
 * -> (state2, T different symbols, T different shifts)
 * each symbol represented by one character in regex
 */
struct Transition {
	unsigned int state1;
	std::string symbols1;
	unsigned int state2;
	std::string symbols2;
	std::vector<int> shifts;

	Transition(const unsigned int state1, const std::string symbols1, const unsigned int state2, const std::string symbols2, const std::vector<int> shifts);
};

#endif
