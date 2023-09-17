#include <cstddef>	// std::size_t
#include <fstream>	// std::ifstream
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule
#include <TMCompiler/compiler/utils/bnf_parser.hpp>		  // BnfParser
#include <TMCompiler/utils/unittesting/unittests.hpp>	  // TEST_CASE, ASSERT

TEST_CASE(test0) {
	std::ifstream input_bnf("TMCompiler/tests/data/example_grammar.bnf");
	std::vector<Rule> rules = BnfParser::parse_rules(input_bnf);

	const std::size_t expected_num_rules = 9;
	ASSERT(rules.size() == expected_num_rules);

	const int five = 5;
	const int six = 6;
	const int seven = 7;
	const int eight = 8;

	int index = 0;
	for(const Rule& rule : rules) {
		const std::string lhs = rule.production.value;
		const std::vector<GrammarSymbol> rhs = rule.replacement;
		if(index == 0) {
			ASSERT(lhs == "P");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "S");
		} else if(index == 1) {
			ASSERT(lhs == "S");
			ASSERT(rhs.size() == 3);

			ASSERT(rhs[0].value == "S");
			ASSERT(rhs[1].value == "+");
			ASSERT(rhs[2].value == "M");
		} else if(index == 2) {
			ASSERT(lhs == "S");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "M");
		} else if(index == 3) {
			ASSERT(lhs == "M");
			ASSERT(rhs.size() == 3);

			ASSERT(rhs[0].value == "M");
			ASSERT(rhs[1].value == "*");
			ASSERT(rhs[2].value == "T");
		} else if(index == 4) {
			ASSERT(lhs == "M");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "T");
		} else if(index == five) {
			ASSERT(lhs == "T");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "1");
		} else if(index == six) {
			ASSERT(lhs == "T");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "2");
		} else if(index == seven) {
			ASSERT(lhs == "T");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "3");
		} else if(index == eight) {
			ASSERT(lhs == "T");
			ASSERT(rhs.size() == 1);

			ASSERT(rhs[0].value == "4");
		}

		++index;
	}
}
