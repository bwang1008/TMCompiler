#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <set>	   // std::set
#include <string>  // std::string
#include <vector>  // std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule
#include <TMCompiler/compiler/models/token.hpp>			  // Token
#include <TMCompiler/compiler/parser/earley_parser.hpp>	  // SubParse

class Grammar {
public:
	explicit Grammar(const std::string& bnf_file, std::string top_symbol);
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
