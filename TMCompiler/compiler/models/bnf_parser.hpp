#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// represent <abc> and "abc"
struct Symbol {
	std::string name;
	bool terminal;
};

// represent <abc> "def" | "ghi" <jkl>
using ReplacementAlternatives = std::vector<std::vector<Symbol> >;

// represent <A> ::= <abc> "def" | "ghi" <jkl>
using Rule = std::pair<Symbol, ReplacementAlternatives>;

// represent many lines of the form <A> ::= <abc> "def" | "ghi" <jkl>
using Rules = std::unordered_map<Symbol, ReplacementAlternatives>;

namespace BnfParser {
// use in Grammar's init: rules = BnfParser::parse_rules("C.bnf")
auto parse_rules(std::ifstream bnf_file_stream) -> Rules;
auto parse_rules(std::string bnf_contents) -> Rules;

const std::string bnf_comment_start = std::string("#");
const std::string bnf_terminal_start = std::string("\"");
const std::string bnf_terminal_end = std::string("\"");
const std::string bnf_nonterminal_start = std::string("<");
const std::string bnf_nonterminal_end = std::string(">");
const std::string bnf_replacement_separation = std::string("::=");
const std::string bnf_choice = std::string("|");
};	// namespace BnfParser

#endif
