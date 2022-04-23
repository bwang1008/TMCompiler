#ifndef TAPE_HPP
#define TAPE_HPP

#include <string>		// std::string
#include <vector>		// std::vector

#include "TMCompiler/utils/json.hpp"

class Tape {
	public:
		Tape();
		Tape(const std::string &s);
		char read() const;
		void write(const char symbol);
		void moveHead(int shift);
		void moveHeadLeft();
		void moveHeadRight();
		void clear();

		int headPosition() const;
		void display() const;
		std::string tapeContents() const;

		// allow these non-member functions to access all members for json serialization
		friend void to_json(nlohmann::json &j, const Tape &tape);
		friend void from_json(const nlohmann::json &j, Tape &tape);

	private:
		std::vector<char> tape;
		std::vector<char> tapeNeg;
		int head;

		int earliestIndex;
		int latestIndex;

		char get(const int index) const;	
};

#endif
