#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <TMCompiler/utils/unittesting/unittests.hpp>
#include <TMCompiler/compiler/models/bnf_parser.hpp>

TEST_CASE(test0) {
	std::ifstream input_bnf("TMCompiler/tests/data/example_grammar.bnf");
	Rules rules = BnfParser::parse_rules(input_bnf);

	ASSERT(rules.size() == 4);

	int index = 0;
	for(std::map<std::string, std::vector<std::vector<Symbol> > >::iterator it = rules.begin(); it != rules.end(); ++it) {
		const std::string lhs = it->first;
		ReplacementAlternatives rhs = it->second;
		if(index == 0) {
			ASSERT(lhs == "M");
			ASSERT(rhs.size() == 2);

			ASSERT(rhs[0].size() == 3);
			ASSERT(rhs[0][0].get_name() == "M");
			ASSERT(rhs[0][1].get_name() == "*");
			ASSERT(rhs[0][2].get_name() == "T");

			ASSERT(rhs[1].size() == 1);
			ASSERT(rhs[1][0].get_name() == "T");
		}
		else if(index == 1) {
			ASSERT(lhs == "P");
			ASSERT(rhs.size() == 1);
			ASSERT(rhs[0].size() == 1);
			ASSERT(rhs[0][0].get_name() == "S");
		}
		else if(index == 2) {
			ASSERT(lhs == "S");
			ASSERT(rhs.size() == 2);

			ASSERT(rhs[0].size() == 3);
			ASSERT(rhs[0][0].get_name() == "S");
			ASSERT(rhs[0][1].get_name() == "+");
			ASSERT(rhs[0][2].get_name() == "M");

			ASSERT(rhs[1].size() == 1);
			ASSERT(rhs[1][0].get_name() == "M");
		}
		else if(index == 3) {
			ASSERT(lhs == "T");
			ASSERT(rhs.size() == 4);

			ASSERT(rhs[0].size() == 1);
			ASSERT(rhs[0][0].get_name() == "1");

			ASSERT(rhs[1].size() == 1);
			ASSERT(rhs[1][0].get_name() == "2");

			ASSERT(rhs[2].size() == 1);
			ASSERT(rhs[2][0].get_name() == "3");

			ASSERT(rhs[3].size() == 1);
			ASSERT(rhs[3][0].get_name() == "4");
		}

		++index;
	}
}
