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
	/**
	 * Constructor for Grammar class.
	 *
	 * @param rules: list of rules: non-terminal symbols to productions
	 * @param default_start: non-terminal symbol name that every compilation
	 * matches
	 */
	Grammar(std::vector<Rule> _rules, std::string _default_start);
	[[nodiscard]] auto parse(const std::vector<Token>& input_tokens) const
		-> std::vector<SubParse>;
	[[nodiscard]] auto get_rules() const -> std::vector<Rule>;
	auto mark_special_symbols_as_terminal(
		const std::set<std::string>& special_tokens) -> void;

private:
	std::vector<Rule> rules;
	std::string default_start;
};

#endif
