#ifndef TAPE_H
#define TAPE_H

#include <string>
#include <vector>

class Tape {
public:
	Tape(const std::string& s, const int offset);
	~Tape();
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
