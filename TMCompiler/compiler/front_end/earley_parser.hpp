/**
 * Earley Parser for parsing context-free-grammars.
 * See https://loup-vaillant.fr/tutorials/earley-parsing/recogniser
 */
#ifndef EARLEY_PARSER_HPP
#define EARLEY_PARSER_HPP

#include <cstddef>	// std::size_t
#include <string>	// std::string
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/token.hpp>			  // Token
#include <TMCompiler/compiler/models/rule.hpp>		// Rule

struct EarleyItem {
	std::size_t rule;	// index of rule in list of rules in Grammar
	std::size_t start;	// index of token where partial match started
	std::size_t next;	// location of dot in right-hand side of rule
};

struct FlippedEarleyItem {
	std::size_t rule;
	std::size_t end;
	std::size_t next;
};

struct SubParse {
	std::size_t rule;
	std::size_t start;
	std::size_t end;
	std::size_t parent;
};

/**
 * Build up the entire Earley state sets from a given input and set of
 * grammar rules. From it, backtrack from the end to find the parse of
 * the entire input program.
 * @param grammar_rules: list of production symbols to replacement rules
 * @param inputs: the "words" of the program / input
 * @param default_start: the top symbol of the parse; which production
 * rule in grammar_rules should start parsing the input
 * @return list of Earley state sets, of size inputs.size() + 1.
 * state_set[i] refers to the valid possible parses, before reading
 * token[i]. The last state set that has a finished rule and starts from
 * the beginning, is a valid grammar parse of the input tokens.
 */
auto build_earley_items(const std::vector<Rule>& grammar_rules,
						const std::vector<Token>& inputs,
						const std::string& default_start)
	-> std::vector<std::vector<EarleyItem> >;

/**
 * Build up parse tree from input_tokens, given partial parses from Earley
 * state sets.
 * @param earley_sets: Earley State sets generated by build_earley_items
 * @param grammar_rules: list of input to replacement symbols from a
 * context-free grammar
 * @param input_tokens: words from the input program
 * @param default_start: the top-level symbol that describes the entire
 * input program
 * @return list of SubParse, each with a range of tokens its rule covers, and
 *		an index of its parent SubParse
 */
auto build_earley_parse_tree(
	const std::vector<std::vector<EarleyItem> >& earley_sets,
	const std::vector<Rule>& grammar_rules,
	const std::vector<Token>& input_tokens,
	const std::string& default_start) -> std::vector<SubParse>;

#endif
