#include <fstream>

#include <TMCompiler/utils/unittesting/unittests.hpp>
#include <TMCompiler/compiler/models/bnf_parser.hpp>

TEST_CASE(test0) {
	std::ifstream input_bnf("TMCompiler/tests/data/example_grammar.bnf");
	Rules rules = BnfParser::parse_rules(input_bnf);

	ASSERT(rules.size() == 4);
}
