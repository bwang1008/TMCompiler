#ifndef TURING_MACHINE_H
#define TURING_MACHINE_H

#include <tuple>		// std::tuple

// interface
class TuringMachine {
public:
	TuringMachine() {}
	virtual ~TuringMachine() {}
	virtual void setInput(const std::string &input) = 0;
	virtual bool halted() const = 0;
	virtual int step(const int verbose=0) = 0;
	virtual std::tuple<int, int> run(const int verbose=0, const int maxSteps=-1) = 0;	
};

#endif
