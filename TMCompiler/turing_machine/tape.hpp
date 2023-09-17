#ifndef TAPE_HPP
#define TAPE_HPP

#include <cstddef>	// std::size_t
#include <vector>	// std::vector

#include "TMCompiler/turing_machine/shift.hpp"
#include "TMCompiler/turing_machine/symbol.hpp"

using Symbol = char;

class Tape {
public:
	Tape();
	auto read() -> Symbol const;
	auto write(Symbol symbol) -> void const;
	auto shiftHead(Shift shift) -> void const;

private:
	std::vector<Symbol> cells;
	std::size_t head;
};

#endif
