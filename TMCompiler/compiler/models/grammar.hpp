#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <fstream>	// std::ifstream
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol

// requires C++17
// see
// https://stackoverflow.com/questions/6517231/are-c-recursive-type-definitions-possible-in-particular-can-i-put-a-vectort
struct AbstractSyntaxTreeNode {
	std::string name;
	std::vector<AbstractSyntaxTreeNode> children;
};

struct AbstractSyntaxTree {
	AbstractSyntaxTreeNode root;
};

class Grammar {
public:
	Grammar(std::ifstream& bnf_file);
	AbstractSyntaxTree parse(const std::vector<char>& program);

private:
	Rules rules;
	std::string default_start;

	AbstractSyntaxTreeNode parse_helper(const std::vector<char>& program,
										int cursor,
										GrammarSymbol symbol);
};

#endif
