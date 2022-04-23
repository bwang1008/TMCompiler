#include "tape.hpp"

#include <algorithm>		// std::max
#include <iostream>			// std::cout, std::endl
#include <string>			// std::string
#include <vector>			// std::vector

#include "TMCompiler/utils/constants.hpp"
#include "TMCompiler/utils/json.hpp"

Tape::Tape() : tape(1, Constants::Symbol::blank), tapeNeg(1, Constants::Symbol::blank), head{0}, earliestIndex{0}, latestIndex{-1} {

}

/**
 * constructor fills in tape with a string, starting at index 0
 */
Tape::Tape(const std::string& s) :
		tape(std::max(s.size(), static_cast<size_t>(1)), Constants::Symbol::blank),
		tapeNeg(1, Constants::Symbol::blank),
		head{0},
		earliestIndex{0},
		latestIndex{static_cast<int>(s.size()) - 1} {
	for(size_t i = 0; i < s.size(); ++i) {
		this->tape[i] = s[i];
	}
}

/**
 * Retrieve character at some index. 
 * If out of bounds, is blank character
 */
char Tape::get(const int index) const {
	const size_t posIndex = static_cast<size_t>((index < 0) ? -index : index);
	if(index < 0) {
		if(posIndex >= this->tapeNeg.size()) {
			return Constants::Symbol::blank;
		}
		return this->tapeNeg[posIndex];
	}
	else {
		if(posIndex >= this->tape.size()) {
			return Constants::Symbol::blank;
		}
		return this->tape[posIndex];
	}
}

/**
 * Retrieve character under head of tape
 * Assume head in valid position
 */
char Tape::read() const {
	return this->get(this->head);
}

/**
 * Write character under head of tape
 * Only allow symbols from Constants: 0, 1, _ (blank)
 * Assume head in valid position
 */
void Tape::write(const char symbol) {
	if(this->head < 0) {
		this->tapeNeg[-(this->head)] = symbol;
	}
	else {
		this->tape[this->head] = symbol;
	}

	if(symbol != Constants::Symbol::blank) {
		// range of non-blank characters could have grown
		if(this->head < this->earliestIndex) {
			this->earliestIndex = this->head;
		}
		if(this->head > this->latestIndex) {
			this->latestIndex = this->head;
		}
	}
	else {
		// is blank: range of non-blank characters could have shrunk
		if(this->head == this->earliestIndex) {
			++this->earliestIndex;
		}
		else if(this->head == this->latestIndex) {
			--this->latestIndex;
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
	--this->head;
	if(this->head < 0 && this->tapeNeg.size() == static_cast<size_t>(-(this->head))) {
		this->tapeNeg.push_back(Constants::Symbol::blank);
	}
}

void Tape::moveHeadRight() {
	++this->head;
	if(this->head >= 0 && this->tape.size() == static_cast<size_t>(this->head)) {
		this->tape.push_back(Constants::Symbol::blank);
	}
}

void Tape::clear() {
	this->tape.clear();
	this->tapeNeg.clear();

	this->tape.push_back(Constants::Symbol::blank);
	this->tapeNeg.push_back(Constants::Symbol::blank);

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

std::string Tape::tapeContents() const {
	std::string ans;
	for(int i = earliestIndex; i <= latestIndex; ++i) {
		ans.push_back(this->get(i));
	}
	return ans;
}

// serialization methods for nlohmann::json
void to_json(nlohmann::json &j, const Tape &tape) {
	j = nlohmann::json{
		{"tape", tape.tape},
		{"tapeNeg", tape.tapeNeg},
		{"head", tape.head},
		{"earliestIndex", tape.earliestIndex},
		{"latestIndex", tape.latestIndex}};
}

void from_json(const nlohmann::json &j, Tape &tape) {
	j.at("tape").get_to(tape.tape);
	j.at("tapeNeg").get_to(tape.tapeNeg);
	j.at("head").get_to(tape.head);
	j.at("earliestIndex").get_to(tape.earliestIndex);
	j.at("latestIndex").get_to(tape.latestIndex);
}
