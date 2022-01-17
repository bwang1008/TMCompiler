#ifndef TAPE_H
#define TAPE_H

#include <string>
#include <vector>

class Tape {
public:
	Tape(const std::string& s, int offset);
	~Tape();
	char read();
	void write(char symbol);
	void moveHead(int shift);
	void moveHeadLeft();
	void moveHeadRight();
	void clear();

	int headPosition();
	void display();

private:
	std::vector<char> tape;
	std::vector<char> tapeNeg;
	int head;

	int earliestIndex;
	int latestIndex;

	char get(int index);	
};

#endif
