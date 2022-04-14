#ifndef MULTI_TAPE_BUILDER_HPP
#define MULTI_TAPE_BUILDER_HPP

#include <string>			// std::string
#include <unordered_map>	// std::unordered_map
#include <utility>			// std::pair
#include <vector>			// std::vector

#include "TMCompiler/tm_definition/tape.hpp"
#include "TMCompiler/tm_definition/transition.hpp"
#include "TMCompiler/tm_definition/multi_tape_turing_machine.hpp"

class MultiTapeBuilder {
	public:
		size_t ipSize;
		size_t numVars;

		MultiTapeBuilder(const std::vector<std::string> &assembly);
		size_t newNode();
		size_t newNode(const std::string &name);
		size_t node(const std::string &name) const;
		size_t tapeIndex(const std::string &tapeName) const;
		void addTransition(const size_t fromState, const size_t toState, const std::vector<std::pair<size_t, std::string> > &reads, const std::vector<std::pair<size_t, std::string> > &writes, const std::vector<std::pair<size_t, int> > &shifts);
		void add1TapeTransition(const size_t fromState, const size_t toState, const size_t tapeIndex, const std::string &read, const std::string &write, const int shift);
		MultiTapeTuringMachine generateMTTM(const size_t initialState, const size_t haltState) const;


	private:
		size_t Q;
		size_t T;
		std::vector<Transition> transitions;
		std::unordered_map<std::string, size_t> tapeIndices;
		std::unordered_map<std::string, size_t> nodeIndices;
};

#endif
