#ifndef TAPE_H
#define TAPE_H

#include <string>
#include <vector>

class Tape {
	std::vector<int> tape;
	std::vector<int> tapeNeg;
	int head;

	int earliestSymbol;
	int latestSymbol;

	int get(int index);	
public:
	Tape(const std::string& s, int offset);
	int read();
	void write(int symbol);
	void moveHead(int shift);
	void moveHeadLeft();
	void moveHeadRight();
	
	int headPosition();
	void display();
};

#endif
