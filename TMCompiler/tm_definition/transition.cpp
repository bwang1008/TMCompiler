#include "transition.hpp"

#include <string>		// std::string

#include "TMCompiler/utils/json.hpp"

Transition::Transition() : state1(0), symbols1("."), state2(0), symbols2("."), shifts(0) {

}

Transition::Transition(const unsigned int state1, const std::string symbols1, const unsigned int state2, const std::string symbols2, const std::vector<int> shifts): state1(state1), symbols1(symbols1), state2(state2), symbols2(symbols2), shifts(shifts) {
	
}

// serialization methods for nlohmann::json
void to_json(nlohmann::json &j, const Transition &transition) {
	j = nlohmann::json{
		{"state1", transition.state1},
		{"symbols1", transition.symbols1},
		{"state2", transition.state2},
		{"symbols2", transition.symbols2},
		{"shifts", transition.shifts}};
}

void from_json(const nlohmann::json &j, Transition &transition) {
	j.at("state1").get_to(transition.state1);
	j.at("symbols1").get_to(transition.symbols1);
	j.at("state2").get_to(transition.state2);
	j.at("symbols2").get_to(transition.symbols2);
	j.at("shifts").get_to(transition.shifts);
}

