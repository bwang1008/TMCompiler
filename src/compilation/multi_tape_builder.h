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

		MultiTapeBuilder(std::vector<std::pair<std::string, int> > &tapeCounts, size_t programSize, size_t numVars);

		int newNode();
		int newNode(const std::string &name);
		int node(const std::string &name) const;
		int tapeIndex(const std::string &tapeName) const;
		void addTransition(int fromState, int toState, std::vector<std::pair<int, std::string> > &reads, std::vector<std::pair<int, std::string> > &writes, std::vector<std::pair<int, int> > &shifts);
		void add1TapeTransition(int fromState, int toState, int tapeIndex, const std::string &read, const std::string &write, int shift);
		MultiTapeTuringMachine generateMTTM(const int initialState, const int haltState) const;


	private:
		int Q;
		int T;
		std::vector<Transition> transitions;
		std::unordered_map<std::string, int> tapeIndices;
		std::unordered_map<std::string, int> nodeIndices;
};

#endif
