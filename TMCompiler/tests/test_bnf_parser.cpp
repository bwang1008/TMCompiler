#include <cstddef>	// std::size_t
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule
#include <TMCompiler/compiler/utils/bnf_parser.hpp>		  // BnfParser

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test0") {
	const std::string input_bnf = "TMCompiler/tests/data/example_grammar.bnf";
	std::vector<Rule> rules = BnfParser::parse_rules(input_bnf);

	const std::size_t expected_num_rules = 9;
	REQUIRE(rules.size() == expected_num_rules);

	const int five = 5;
	const int six = 6;
	const int seven = 7;
	const int eight = 8;

	int index = 0;
	for(const Rule& rule : rules) {
		const std::string lhs = rule.production.value;
		const std::vector<GrammarSymbol> rhs = rule.replacement;
		if(index == 0) {
			REQUIRE(lhs == "P");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "S");
		} else if(index == 1) {
			REQUIRE(lhs == "S");
			REQUIRE(rhs.size() == 3);

			REQUIRE(rhs[0].value == "S");
			REQUIRE(rhs[1].value == "+");
			REQUIRE(rhs[2].value == "M");
		} else if(index == 2) {
			REQUIRE(lhs == "S");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "M");
		} else if(index == 3) {
			REQUIRE(lhs == "M");
			REQUIRE(rhs.size() == 3);

			REQUIRE(rhs[0].value == "M");
			REQUIRE(rhs[1].value == "*");
			REQUIRE(rhs[2].value == "T");
		} else if(index == 4) {
			REQUIRE(lhs == "M");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "T");
		} else if(index == five) {
			REQUIRE(lhs == "T");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "1");
		} else if(index == six) {
			REQUIRE(lhs == "T");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "2");
		} else if(index == seven) {
			REQUIRE(lhs == "T");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "3");
		} else if(index == eight) {
			REQUIRE(lhs == "T");
			REQUIRE(rhs.size() == 1);

			REQUIRE(rhs[0].value == "4");
		}

		++index;
	}
}
