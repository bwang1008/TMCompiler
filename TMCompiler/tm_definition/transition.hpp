#ifndef TRANSITION_HPP
#define TRANSITION_HPP

#include <string>		// std::string
#include <vector>		// std::vector

#include "TMCompiler/utils/json.hpp"

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

	Transition();
	Transition(const unsigned int state1, const std::string symbols1, const unsigned int state2, const std::string symbols2, const std::vector<int> shifts);
	
	// allow these non-member functions to access all members for json serialization
	friend void to_json(nlohmann::json &j, const Transition &transition);
	friend void from_json(const nlohmann::json &j, Transition &transition);
};

#endif
