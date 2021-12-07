#ifndef TAPE_H
#define TAPE_H

#include <string>
#include <vector>

class Tape {
	std::vector<int> tape;
	std::vector<int> tapeNeg;
	int head;

public:
	Tape(const std::string& s, int offset);
	int read();
	void write(int symbol);
	void moveHeadLeft();
	void moveHeadRight();
};

#endif
