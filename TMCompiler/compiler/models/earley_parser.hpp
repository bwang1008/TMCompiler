/**
 * Earley Parser for parsing context-free-grammars.
 * See https://loup-vaillant.fr/tutorials/earley-parsing/recogniser
 */
#ifndef EARLEY_PARSER_HPP
#define EARLEY_PARSER_HPP

#include <TMCompiler/compiler/models/bnf_parser.hpp>  // Symbol
#include <TMCompiler/compiler/models/tokenizer.hpp>	  // Token

struct EarleyItem {
	std::size_t rule;	// index of rule in list of rules in Grammar
	std::size_t start;	// index of token where partial match started
	std::size_t next;	// location of dot in right-hand side of rule
};

struct EarleyRule {
	Symbol production;
	std::vector<Symbol> replacement;
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
auto build_earley_items(std::vector<EarleyRule> grammar_rules,
						std::vector<Token> inputs,
						const std::string& default_start)
	-> std::vector<std::vector<EarleyItem> >;

#endif