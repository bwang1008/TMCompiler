#include <TMCompiler/compiler/front_end/read_grammar.hpp>  // read_grammar()
#include <TMCompiler/utils/unittesting/unittests.hpp>

// g++ -std=c++11 -I. TMCompiler/tests/test_grammar_parser.cpp

TEST_CASE(test0) {
	Grammar grammar = read_grammar("data/example_grammar.bnf");

	ASSERT(0 == 0);
}
