#ifndef BNF_PARSER_HPP
#define BNF_PARSER_HPP

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

// represent <abc> and "abc"
class Symbol {
private:
	std::string name;
	bool terminal{false};

public:
	Symbol();
	Symbol(std::string name, bool terminal);
	auto get_name() -> std::string;
};

// represent <abc> "def" | "ghi" <jkl>
using ReplacementAlternatives = std::vector<std::vector<Symbol> >;

// represent <A> ::= <abc> "def" | "ghi" <jkl>
using Rule = std::pair<std::string, ReplacementAlternatives>;

// represent many lines of the form <A> ::= <abc> "def" | "ghi" <jkl>
using Rules = std::map<std::string, ReplacementAlternatives>;

namespace BnfParser {
// use in Grammar's init: rules = BnfParser::parse_rules(std::ifstream("C.bnf"))
auto parse_rules(std::ifstream& bnf_file_stream) -> Rules;
auto parse_rules(const std::string& bnf_contents) -> Rules;

const std::string bnf_comment_start = std::string("#");
const std::string bnf_terminal_start = std::string("\"");
const std::string bnf_terminal_end = std::string("\"");
const std::string bnf_nonterminal_start = std::string("<");
const std::string bnf_nonterminal_end = std::string(">");
const std::string bnf_replacement_separation = std::string("::=");
const std::string bnf_choice = std::string("|");
};	// namespace BnfParser

#endif
