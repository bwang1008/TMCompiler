#include "transition.h"

#include <string>		// std::string
#include <utility>		// std::move

Transition::Transition(const int state1, const std::string &&symbols1, const int state2, const std::string &&symbols2, const std::vector<int> &&shifts): state1(state1), symbols1(std::move(symbols1)), state2(state2), symbols2(std::move(symbols2)), shifts(std::move(shifts)) {
	
}

Transition::Transition(const int state1, const char symbols1[], const int state2, const char symbols2[], const std::vector<int> &&shifts) : state1(state1), symbols1(std::string(symbols1)), state2(state2), symbols2(std::string(symbols2)), shifts(std::move(shifts)) {

}


bool Transition::operator< (const Transition other) const {
	if(state1 == other.state1) {
		return symbols1 < other.symbols1;
	}

	return state1 < other.state1;
}
