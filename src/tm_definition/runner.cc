#include "config.h"
#include "tape.h"

#include <iostream>

int main() {
	Tape* tape = new Tape("abacd", -3);
	
	int maxRepetitions = 20;
	int repetition = 0;
	
	int c = tape->read();
	while(c != config::blank && repetition < maxRepetitions) {
		std::cout << "read " << char(c) << std::endl;
		tape->moveHeadRight();
		c = tape->read();

		repetition++;
	}

	

}
