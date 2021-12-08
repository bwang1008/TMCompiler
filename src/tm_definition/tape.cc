#include "config.h"
#include "tape.h"

#include <algorithm>		// max
#include <string>			// string
#include <iostream>			// cout, endl

Tape::Tape(const std::string& s, int offset) : tape(std::max(int(offset + s.size()), 0), Config::blank), tapeNeg(std::max(-offset + 1, 0), Config::blank), head{offset} {
	for(int i = 0; i < s.size(); i++) {
		char c = s[i];
		int index = offset + i;
		if(index < 0) {
			tapeNeg[-index] = c;
		}
		else {
			tape[index] = c;
		}
	}

	earliestSymbol = offset;
	latestSymbol = offset + s.size() - 1;
}

int Tape::get(int index) {
	if(index < 0) {
		if(-index >= tapeNeg.size()) {
			return Config::blank;
		}
		return tapeNeg[-index];
	}
	else {
		if(index >= tape.size()) {
			return Config::blank;
		}
		return tape[index];
	}
}

// assume head in valid position
int Tape::read() {
	return get(head);
}

// assume head in valid position
void Tape::write(int symbol) {
	if(head < 0) {
		tapeNeg[-head] = symbol;
	}
	else {
		tape[head] = symbol;
	}

	if(symbol != Config::blank) {
		if(head < earliestSymbol) {
			earliestSymbol = head;
		}
		if(head > latestSymbol) {
			latestSymbol = head;
		}
	}
	else {
		// is blank
		if(head == earliestSymbol) {
			earliestSymbol++;
		}
		else if(head == latestSymbol) {
			latestSymbol--;
		}
	}
}

void Tape::moveHead(int shift) {
	if(shift == Config::Shift::left) {
		moveHeadLeft();
	}
	else if(shift == Config::Shift::right) {
		moveHeadRight();
	}
}

void Tape::moveHeadLeft() {
	head--;
	if(head < 0 && tapeNeg.size() == -head) {
		tapeNeg.push_back(Config::blank);
	}
}

void Tape::moveHeadRight() {
	head++;
	if(head >= 0 && tape.size() == head) {
		tape.push_back(Config::blank);
	}
}

int Tape::headPosition() {
	return head;
}

void Tape::display() {
	std::cout << earliestSymbol << ":[";
	for(int i = earliestSymbol; i <= latestSymbol; i++) {
		std::cout << get(i);
		if(i < latestSymbol) {
			std::cout << ", ";
		}
	}
	std::cout << "]\n";
}
