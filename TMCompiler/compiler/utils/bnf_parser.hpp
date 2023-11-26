#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

#include <string>		// std::string
#include <string_view>	// std::string_view
#include <vector>		// std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule

// represent <abc> "def" | "ghi" <jkl>
using ReplacementAlternatives = std::vector<std::vector<GrammarSymbol> >;

namespace BnfParser {
// use in Grammar's init:
// rules = BnfParser::parse_rules("C.bnf");
auto parse_rules(const std::string& bnf_file) -> std::vector<Rule>;
auto parse_rules_contents(std::string_view bnf_contents) -> std::vector<Rule>;

const std::string bnf_comment_start = "#";
const std::string bnf_terminal_start = "\"";
const std::string bnf_terminal_end = "\"";
const std::string bnf_nonterminal_start = "<";
const std::string bnf_nonterminal_end = ">";
const std::string bnf_replacement_separation = "::=";
const std::string bnf_choice = "|";
}  // namespace BnfParser

#endif
