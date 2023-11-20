#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

#include <string>
#include <vector>

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule

// represent <abc> "def" | "ghi" <jkl>
using ReplacementAlternatives = std::vector<std::vector<GrammarSymbol> >;

namespace BnfParser {
// use in Grammar's init:
// rules = BnfParser::parse_rules("C.bnf");
auto parse_rules(const std::string& bnf_file) -> std::vector<Rule>;
auto parse_rules_contents(const std::string& bnf_contents) -> std::vector<Rule>;

const std::string bnf_comment_start = std::string("#");
const std::string bnf_terminal_start = std::string("\"");
const std::string bnf_terminal_end = std::string("\"");
const std::string bnf_nonterminal_start = std::string("<");
const std::string bnf_nonterminal_end = std::string(">");
const std::string bnf_replacement_separation = std::string("::=");
const std::string bnf_choice = std::string("|");
}  // namespace BnfParser

#endif
