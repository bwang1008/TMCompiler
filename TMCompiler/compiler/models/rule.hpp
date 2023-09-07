#ifndef RULE_HPP
#define RULE_HPP

#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol

struct Rule {
	GrammarSymbol production;
	std::vector<GrammarSymbol> replacement;
};

#endif
