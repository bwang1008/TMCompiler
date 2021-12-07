#include "config.h"
#include "tape.h"

#include <algorithm>		// max
#include <string>			// string
#include <iostream>			// cout, endl

Tape::Tape(const std::string& s, int offset) : tape(std::max(int(offset + s.size()), 0), config::blank), tapeNeg(std::max(-offset + 1, 0), config::blank), head{offset} {
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
}

// assume head in valid position
int Tape::read() {
	if(head < 0) {
		return tapeNeg[-head];
	}
	return tape[head];
}

// assume head in valid position
void Tape::write(int symbol) {
	if(head < 0) {
		tapeNeg[-head] = symbol;
	}
	else {
		tape[head] = symbol;
	}
}

void Tape::moveHeadLeft() {
	head--;
	if(head < 0 && tapeNeg.size() == -head) {
		tapeNeg.push_back(config::blank);
	}
}

void Tape::moveHeadRight() {
	head++;
	if(head >= 0 && tape.size() == head) {
		tape.push_back(config::blank);
	}
}
