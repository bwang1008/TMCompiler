#include "multi_tape_builder.h"
#include "../tm_definition/transition.h"
#include "../tm_definition/constants.h"

MultiTapeBuilder::MultiTapeBuilder(std::vector<std::pair<std::string, int> > &tapeCounts, size_t ipSize, size_t numVars) {
	int numTapes = 0;
	for(size_t i = 0; i < tapeCounts.size(); ++i) {
		std::pair<std::string, int> p = tapeCounts[i];
		std::string tapeName = p.first;
		int count = p.second;

		this.tapeIndices[tapeName] = numTapes;
		numTapes += count;
	}
	
	this.T = numTapes;
	this.Q = 0;

	this.ipSize = ipSize;
	this.numVars = numVars;
}

int MultiTapeBuilder::newNode(std::string &name) {
	int numNodes = this.Q;
	++Q;

	if(name.size() != 0) {
		this.nodeIndices[name] = numNodes;
	}

	return numNodes;
}

int MultiTapeBuilder::node(std::string &name) {
	if(this.nodeIndices.find(name) != this.nodeIndices.end()) {
		std::cout << "Node " << name << " not found" << std::endl;
		return -1;
	}
	
	return this.nodeIndices[name];
}

int MultiTapeBuilder::tapeIndices(std::string &tapeName) {
	if(this.tapeIndices.find(tapeName) != this.tapeIndices.end()) {
		std::cout << "Node " << tapeName << " not found" << std::endl;
		return -1;
	}

	return this.tapeIndices[tapeName];
}

void MultiTapeBuilder::addTransition(int fromState, int toState, std::vector<std::pair<int, std::string> > &reads, std::vector<std::vector<std::pair<int, std::string> > &writes, std::vector<std::pair<int, int> > &shifts) {
	std::vector<std::string> tapeRead(this.T, ".");
	std::vector<std::string> tapeWrite(this.T, ".");

	for(size_t i = 0; i < reads.size(); ++i) {
		std::pair<int, std::string> p = reads[i];
		int tapeIndex = p.first;
		std::string rule = p.second;
		tapeRead[tapeIndex] = rule;
	}

	for(size_t i = 0; i < writes.size(); ++i) {
		std::pair<int, std::string> p = writes[i];
		int tapeIndex = p.first;
		std::string rule = p.second;
		tapeWrite[tapeIndex] = rule;
	}

	std::string allReadRules;
	std::string allWriteRules;
	
	for(size_t i = 0; i < tapeRead.size(); ++i) {
		allReadRules.push_back(tapeRead[i]);
	}

	for(size_t i = 0; i < tapeWrite.size(); ++i) {
		allWriteRules.push_back(tapeWrite[i]);
	}
	
	std::vector<int> allShifts(this.T, Constants::Shift::none);
	for(size_t i = 0; i < shifts.size(); ++i) {
		std::pair<int, int> p = shifts[i];
		int index = p.first;
		int shift = p.second;

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

void MultiTapeBuilder::add1TapeTransition(int fromState, int toState, int tapeIndex, std::string &read, std:string &write, int shift) {
	std::vector<std::pair<int, std::string> > reads;
	std::vector<std::pair<int, std::string> > writes;
	std::vector<std::pair<int, int> > shifts;

	reads.emplace_back(tapeIndex, read);
	writes.emplace_back(tapeIndex, write);
	shifts.emplace_back(tapeIndex, shift);

	addTransition(fromState, toState, reads, writes, shifts);

}
