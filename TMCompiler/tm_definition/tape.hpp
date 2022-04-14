#ifndef TAPE_HPP
#define TAPE_HPP

#include <string>		// std::string
#include <vector>		// std::vector

class Tape {
public:
	Tape(const std::string &s);
	char read() const;
	void write(const char symbol);
	void moveHead(int shift);
	void moveHeadLeft();
	void moveHeadRight();
	void clear();

	int headPosition() const;
	void display() const;

private:
	std::vector<char> tape;
	std::vector<char> tapeNeg;
	int head;

	int earliestIndex;
	int latestIndex;

	char get(const int index) const;	
};

#endif
