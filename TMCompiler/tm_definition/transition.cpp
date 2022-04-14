#include "transition.hpp"

#include <string>		// std::string

Transition::Transition(const unsigned int state1, const std::string symbols1, const unsigned int state2, const std::string symbols2, const std::vector<int> shifts): state1(state1), symbols1(symbols1), state2(state2), symbols2(symbols2), shifts(shifts) {
	
}
