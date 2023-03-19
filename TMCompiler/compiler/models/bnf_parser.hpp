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

class BnfParser {
public:
	BnfParser();
	Rules parse_rules(std::ifstream bnf_file_stream);
	Rules parse_rules(const std::string bnf_contents);

private:

	// use in Grammar's init: BnfParser("C.bnf").getRules()
	const char* bnf_comment_start = "#";
	const char* bnf_terminal_start = "\"";
	const char* bnf_terminal_end = "\"";
	const char* bnf_nonterminal_start = "<";
	const char* bnf_nonterminal_end = ">";
	const char* bnf_replacement_separation = "::=";
	const char* bnf_choice = "|";

	std::pair<Symbol, std::size_t> parse_symbol(const std::string bnf_contents, const std::size_t start_index);
};

#endif
