#ifndef MULTI_TAPE_BUILDER_H
#define MULTI_TAPE_BUILDER_H

#include <unordered_map>	// std::unordered_map
#include <utility>			// std::pair
#include <string>			// std::string
#include <vector>			// std::vector

#include "../tm_definition/tape.h"
#include "../tm_definition/transition.h"
#include "../tm_definition/multi_tape_turing_machine.h"

class MultiTapeBuilder {
	public:
		size_t ipSize;
		size_t numVars;

		MultiTapeBuilder(const std::vector<std::string> &assembly);

		size_t newNode();
		size_t newNode(const std::string &name);
		size_t node(const std::string &name) const;
		size_t tapeIndex(const std::string &tapeName) const;
		void addTransition(const int fromState, const int toState, const std::vector<std::pair<int, std::string> > &reads, const std::vector<std::pair<int, std::string> > &writes, const std::vector<std::pair<int, int> > &shifts);
		void add1TapeTransition(const int fromState, const int toState, const int tapeIndex, const std::string &read, const std::string &write, const int shift);
		MultiTapeTuringMachine generateMTTM(const int initialState, const int haltState) const;


	private:
		size_t Q;
		size_t T;
		std::vector<Transition> transitions;
		std::unordered_map<std::string, int> tapeIndices;
		std::unordered_map<std::string, int> nodeIndices;
};

#endif
