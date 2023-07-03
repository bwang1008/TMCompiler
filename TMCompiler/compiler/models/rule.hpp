#ifndef RULE_HPP
#define RULE_HPP

struct EarleyRule {
	GrammarSymbol production;
	std::vector<GrammarSymbol> replacement;
};

#endif
