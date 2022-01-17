#include "constants.hpp"
#include "tape.hpp"

#include <algorithm>		// max
#include <string>			// string
#include <iostream>			// cout, endl

Tape::Tape(const std::string& s, int offset) : tape(std::max(int(offset + s.size()), 0), Constants::blank), tapeNeg(std::max(-offset + 1, 1), Constants::blank), head{offset} {
	for(size_t i = 0; i < s.size(); i++) {
		char c = s[i];
		int index = offset + i;
		if(index < 0) {
			tapeNeg[-index] = c;
		}
		else {
			tape[index] = c;
		}
	}

	this->earliestIndex = offset;
	this->latestIndex = offset + s.size() - 1;
}

Tape::~Tape() {
	this->clear();
}

char Tape::get(int index) {
	if(index < 0) {
		if((size_t) (-index) >= tapeNeg.size()) {
			return Constants::blank;
		}
		return tapeNeg[-index];
	}
	else {
		if((size_t) (index) >= tape.size()) {
			return Constants::blank;
		}
		return tape[index];
	}
}

// assume head in valid position
char Tape::read() {
	return get(head);
}

// assume head in valid position
void Tape::write(char symbol) {

	if(head < 0) {
		tapeNeg[-head] = symbol;
	}
	else {
		tape[head] = symbol;
	}

	if(symbol != Constants::blank) {
		if(head < earliestIndex) {
			earliestIndex = head;
		}
		if(head > latestIndex) {
			latestIndex = head;
		}
	}
	else {
		// is blank
		if(head == earliestIndex) {
			earliestIndex++;
		}
		else if(head == latestIndex) {
			latestIndex--;
		}
	}
}

void Tape::moveHead(int shift) {
	if(shift == Constants::Shift::left) {
		moveHeadLeft();
	}
	else if(shift == Constants::Shift::right) {
		moveHeadRight();
	}
}

void Tape::moveHeadLeft() {
	head--;
	if(head < 0 && tapeNeg.size() == (size_t) (-head)) {
		tapeNeg.push_back(Constants::blank);
	}
}

void Tape::moveHeadRight() {
	head++;
	if(head >= 0 && tape.size() == (size_t) head) {
		tape.push_back(Constants::blank);
	}
}

void Tape::clear() {
	tape.clear();
	tapeNeg.clear();
	char c = Constants::blank;
	tape.push_back(c);
	tapeNeg.push_back(c);
	head = 0;
	
	earliestIndex = 0;
	latestIndex = 0;
}

int Tape::headPosition() {
	return head;
}

void Tape::display() {
	std::cout << earliestIndex << ": [";
	for(int i = earliestIndex; i <= latestIndex; i++) {
		char symbol = get(i);
		std::cout << char(symbol);
	}
	std::cout << "]\n";
}
