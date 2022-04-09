#include "multi_tape_builder.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "utils.h"
#include "../tm_definition/transition.h"
#include "../tm_definition/constants.h"
#include "../tm_definition/multi_tape_turing_machine.h"


/**
 * Return number of distinct variables of form "!TAPE_tapeX" for some integer X
 */
size_t countTapeVariables(const std::vector<std::string> &assembly) {
	size_t numVars = 0;

	for(size_t i = 0; i < assembly.size(); ++i) {
		const std::vector<std::string> words = getWords(assembly[i]);
		for(size_t j = 0; j < words.size(); ++j) {
			const std::string word = words[j];
			if(word.substr(0, 10) == "!TAPE_tape") {
				const size_t tapeIndex = parseTapeNum(word);
				if(tapeIndex > numVars) {
					numVars = tapeIndex;
				}
			}
		}
	}

	return numVars + 1;
}

MultiTapeBuilder::MultiTapeBuilder(const std::vector<std::string> &assembly) {
	this->numVars = countTapeVariables(assembly);

	this->ipSize = 1;
	int s = ((int) assembly.size()) - 1;	
	while(s > 1) {
		++this->ipSize;
		s /= 2;
	}
	if(this->ipSize < 2) {
		this->ipSize = 2;
	}

	// initialize tapes with names
	std::vector<std::pair<std::string, size_t> > tapeCounts;
	
	tapeCounts.emplace_back("input", 1);
	tapeCounts.emplace_back("output", 1);
	tapeCounts.emplace_back("ipStack", 1);
	tapeCounts.emplace_back("ip", 1);
	tapeCounts.emplace_back("ipSideways", this->ipSize);
	tapeCounts.emplace_back("paramStack", 1);
	tapeCounts.emplace_back("bitIndex", 1);
	tapeCounts.emplace_back("bits", 1);
	tapeCounts.emplace_back("variables", this->numVars);
	tapeCounts.emplace_back("rax", 1);

	size_t numTapes = 0;
	for(size_t i = 0; i < tapeCounts.size(); ++i) {
		std::pair<std::string, size_t> p = tapeCounts[i];
		std::string tapeName = p.first;
		size_t count = p.second;

		this->tapeIndices[tapeName] = numTapes;
		numTapes += count;
	}
	
	this->T = numTapes;
	this->Q = 0;
}

size_t MultiTapeBuilder::newNode() {
	const size_t numNodes = this->Q;
	++Q;

	return numNodes;
}

size_t MultiTapeBuilder::newNode(const std::string &name) {
	if(this->nodeIndices.find(name) != this->nodeIndices.end()) {
		throw std::invalid_argument("Node " + name + " already defined");
	}

	const size_t ans = this->newNode();

	this->nodeIndices[name] = ans;

	return ans;
}

size_t MultiTapeBuilder::node(const std::string &name) const {
	if(this->nodeIndices.find(name) == this->nodeIndices.end()) {
		throw std::invalid_argument("Node " + name + " not found");
	}
	
	return this->nodeIndices.at(name);
}

size_t MultiTapeBuilder::tapeIndex(const std::string &tapeName) const {
	if(this->tapeIndices.find(tapeName) == this->tapeIndices.end()) {
		throw std::invalid_argument("Tape " + tapeName + " not found");
	}

	return this->tapeIndices.at(tapeName);
}

void MultiTapeBuilder::addTransition(const size_t fromState, const size_t toState, const std::vector<std::pair<size_t, std::string> > &reads, const std::vector<std::pair<size_t, std::string> > &writes, const std::vector<std::pair<size_t, int> > &shifts) {
	std::vector<std::string> tapeRead(this->T, ".");
	std::vector<std::string> tapeWrite(this->T, ".");

	for(size_t i = 0; i < reads.size(); ++i) {
		const std::pair<size_t, std::string> p = reads[i];
		const size_t tapeIndex = p.first;
		const std::string rule = p.second;
		tapeRead[tapeIndex] = rule;
	}

	for(size_t i = 0; i < writes.size(); ++i) {
		const std::pair<size_t, std::string> p = writes[i];
		const size_t tapeIndex = p.first;
		const std::string rule = p.second;
		tapeWrite[tapeIndex] = rule;
	}

	std::string allReadRules;
	std::string allWriteRules;
	
	for(size_t i = 0; i < tapeRead.size(); ++i) {
		allReadRules.append(tapeRead[i]);
	}

	for(size_t i = 0; i < tapeWrite.size(); ++i) {
		allWriteRules.append(tapeWrite[i]);
	}
	
	std::vector<int> allShifts(this->T, Constants::Shift::none);
	for(size_t i = 0; i < shifts.size(); ++i) {
		const std::pair<size_t, int> p = shifts[i];
		const size_t index = p.first;
		const int shift = p.second;

		if(shift == 0) {
			allShifts[i] = Constants::Shift::none;
		}
		else if(shift == -1) {
			allShifts[i] = Constants::Shift::left;
		}
		else if(shift == 1) {
			allShifts[i] = Constants::Shift::right;
		}
	}
	
	transitions.emplace_back(fromState, allReadRules, toState, allWriteRules, allShifts);
}

void MultiTapeBuilder::add1TapeTransition(const size_t fromState, const size_t toState, const size_t tapeIndex, const std::string &read, const std::string &write, const int shift) {
	std::vector<std::pair<size_t, std::string> > reads;
	std::vector<std::pair<size_t, std::string> > writes;
	std::vector<std::pair<size_t, int> > shifts;

	reads.emplace_back(tapeIndex, read);
	writes.emplace_back(tapeIndex, write);
	shifts.emplace_back(tapeIndex, shift);

	addTransition(fromState, toState, reads, writes, shifts);
}


MultiTapeTuringMachine MultiTapeBuilder::generateMTTM(const size_t initialState, const size_t haltState) const {
	return MultiTapeTuringMachine(this->Q, this->T, initialState, haltState, this->transitions);
}
