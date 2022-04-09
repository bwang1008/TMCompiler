#include "constants.h"
#include "tape.h"

#include <algorithm>		// max
#include <string>			// string
#include <iostream>			// cout, endl

Tape::Tape(const std::string& s, const int offset) : tape(std::max(int(offset + s.size()), 1), Constants::blank), tapeNeg(std::max(-offset + 1, 1), Constants::blank), head{offset} {
	for(size_t i = 0; i < s.size(); i++) {
		char c = s[i];
		int index = offset + i;
		if(index < 0) {
			this->tapeNeg[-index] = c;
		}
		else {
			this->tape[index] = c;
		}
	}

	this->earliestIndex = offset;
	this->latestIndex = offset + s.size() - 1;
}

Tape::~Tape() {
	this->clear();
}

char Tape::get(const int index) const {
	if(index < 0) {
		if((size_t) (-index) >= this->tapeNeg.size()) {
			return Constants::blank;
		}
		return this->tapeNeg[-index];
	}
	else {
		if((size_t) (index) >= this->tape.size()) {
			return Constants::blank;
		}
		return this->tape[index];
	}
}

// assume head in valid position
char Tape::read() const {
	return this->get(this->head);
}

// assume head in valid position
void Tape::write(const char symbol) {
	if(this->head < 0) {
		this->tapeNeg[-(this->head)] = symbol;
	}
	else {
		this->tape[this->head] = symbol;
	}

	if(symbol != Constants::blank) {
		if(this->head < earliestIndex) {
			this->earliestIndex = this->head;
		}
		if(this->head > this->latestIndex) {
			this->latestIndex = this->head;
		}
	}
	else {
		// is blank
		if(this->head == this->earliestIndex) {
			this->earliestIndex++;
		}
		else if(this->head == this->latestIndex) {
			this->latestIndex--;
		}
	}
}

void Tape::moveHead(int shift) {
	if(shift == Constants::Shift::left) {
		this->moveHeadLeft();
	}
	else if(shift == Constants::Shift::right) {
		this->moveHeadRight();
	}
}

void Tape::moveHeadLeft() {
	this->head--;
	if(this->head < 0 && this->tapeNeg.size() == (size_t) (-(this->head))) {
		this->tapeNeg.push_back(Constants::blank);
	}
}

void Tape::moveHeadRight() {
	this->head++;
	if(this->head >= 0 && this->tape.size() == (size_t) this->head) {
		this->tape.push_back(Constants::blank);
	}
}

void Tape::clear() {
	this->tape.clear();
	this->tapeNeg.clear();
	char c = Constants::blank;
	this->tape.push_back(c);
	this->tapeNeg.push_back(c);
	this->head = 0;
	
	this->earliestIndex = 0;
	this->latestIndex = 0;
}

int Tape::headPosition() const {
	return this->head;
}

void Tape::display() const {
	std::cout << this->head << ": [";
	for(int i = 0; i <= this->latestIndex; i++) {
		char symbol = this->get(i);
		std::cout << char(symbol);
	}
	std::cout << "]\n";
}
