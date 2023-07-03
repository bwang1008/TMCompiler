#ifndef RULE_HPP
#define RULE_HPP

struct Rule {
	GrammarSymbol production;
	std::vector<GrammarSymbol> replacement;
};

#endif
