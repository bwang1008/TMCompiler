#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <TMCompiler/compiler/front_end/bnf_parser.hpp>
#include <TMCompiler/utils/unittesting/unittests.hpp>

TEST_CASE(test0) {
	std::ifstream input_bnf("TMCompiler/tests/data/example_grammar.bnf");
	Rules rules = BnfParser::parse_rules(input_bnf);

	ASSERT(rules.size() == 4);

	int index = 0;
	for(std::map<std::string, std::vector<std::vector<Symbol> > >::iterator it =
			rules.begin();
		it != rules.end();
		++it) {
		const std::string lhs = it->first;
		ReplacementAlternatives rhs = it->second;
		if(index == 0) {
			ASSERT(lhs == "M");
			ASSERT(rhs.size() == 2);

			ASSERT(rhs[0].size() == 3);
			ASSERT(rhs[0][0].value == "M");
			ASSERT(rhs[0][1].value == "*");
			ASSERT(rhs[0][2].value == "T");

			ASSERT(rhs[1].size() == 1);
			ASSERT(rhs[1][0].value == "T");
		} else if(index == 1) {
			ASSERT(lhs == "P");
			ASSERT(rhs.size() == 1);
			ASSERT(rhs[0].size() == 1);
			ASSERT(rhs[0][0].value == "S");
		} else if(index == 2) {
			ASSERT(lhs == "S");
			ASSERT(rhs.size() == 2);

			ASSERT(rhs[0].size() == 3);
			ASSERT(rhs[0][0].value == "S");
			ASSERT(rhs[0][1].value == "+");
			ASSERT(rhs[0][2].value == "M");

			ASSERT(rhs[1].size() == 1);
			ASSERT(rhs[1][0].value == "M");
		} else if(index == 3) {
			ASSERT(lhs == "T");
			ASSERT(rhs.size() == 4);

			ASSERT(rhs[0].size() == 1);
			ASSERT(rhs[0][0].value == "1");

			ASSERT(rhs[1].size() == 1);
			ASSERT(rhs[1][0].value == "2");

			ASSERT(rhs[2].size() == 1);
			ASSERT(rhs[2][0].value == "3");

			ASSERT(rhs[3].size() == 1);
			ASSERT(rhs[3][0].value == "4");
		}

		++index;
	}
}
