#ifndef TAPE_HPP
#define TAPE_HPP

#include <cstddef>	// std::size_t
#include <vector>	// std::vector

#include "TMCompiler/src/turing-machine/shift.hpp"
#include "TMCompiler/src/turing-machine/symbol.hpp"

using Symbol = char;

class Tape {
public:
	Tape();
	Symbol read() const;
	void write(Symbol symbol) const;
	void shiftHead(Shift shift) const;

private:
	std::vector<Symbol> cells;
	std::size_t head;
};

#endif
