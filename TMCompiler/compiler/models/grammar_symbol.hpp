#ifndef GRAMMAR_SYMBOL
#define GRAMMAR_SYMBOL

#include <string>	// std::string

// represent <abc> and "abc" in BNF file
struct GrammarSymbol {
	std::string value;
	bool terminal{false};
};

#endif
