#ifndef MULTI_TAPE_BUILDER_H
#define MULTI_TAPE_BUILDER_H

#include "../tm_definition/tape.h"
#include "../tm_definition/transition.h"

#include <unordered_map>	// std::unordered_map
#include <pair>				// std::pair
#include <string>			// std::string
#include <vector>			// std::vector

class MultiTapeBuilder {
	public:
		MultiTapeBuilder(std::vector<std::pair<std::string, int> > &tapeCounts);

		int newNode(std::string &name = "");
		int node(std::string &name);
		int tapeIndex(std::string &tapeName);
		void addTransition(int fromState, int toState, std::vector<std::pair<int, std::string> > &reads, std::vector<std::vector<std::pair<int, std::string> > &writes, std::vector<std::pair<int, int> > &shifts);
		void add1TapeTransition(int fromState, int toState, int tapeIndex, std::string &read, std:string &write, int shift);


	private:
		int Q;
		int T;
		size_t ipSize;
		std::vector<Transition> transitions;
		std::unordered_map<std::string, int> tapeIndices;
		std::unordered_map<std::string, int> nodeIndices;
};

#endif
