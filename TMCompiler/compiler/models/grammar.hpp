#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <fstream>	// std::ifstream
#include <set>		// std::set
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule
#include <TMCompiler/compiler/models/token.hpp>			  // Token
#include <TMCompiler/compiler/parser/earley_parser.hpp>	  // SubParse

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
	explicit Grammar(std::ifstream& bnf_file, std::string top_symbol);
	auto parse(const std::vector<Token>& input_tokens) const
		-> std::vector<SubParse>;
	auto get_rules() const -> std::vector<Rule>;
	auto mark_special_symbols_as_terminal(
		const std::set<std::string>& special_tokens) -> void;

private:
	std::vector<Rule> rules;
	std::string default_start;

	// auto parse_helper(const std::vector<char>& program,
	// int cursor,
	// GrammarSymbol symbol) -> AbstractSyntaxTreeNode;
};

#endif
