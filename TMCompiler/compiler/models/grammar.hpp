#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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
	Grammar(const std::string& bnf_file_name);
	Grammar(std::unordered_map<Symbol, std::vector<std::vector<Symbol> > > rules, std::string default_start);
	AbstractSyntaxTree parse(const std::vector<char>& program);


private:
	// A nonterminal on the left, can map to many potential replacements,
	// where each replacement is a sequence of symbols
	// Given left-hand non-terminal <A>, what can it map to?
	// Ex: <A> ::= <B> <C> | <D> <E> 
	// becomes
	// { Symbol("A", false) : [[Symbol("B"), Symbol("C")], [Symbol("D"), Symbol("E")]] }
	std::unordered_map < Symbol, std::vector<std::vector<Symbol> > rules;
	std::string default_start;


	// given lines of a BNF file, remove comments and empty lines
	std::vector<std::string> sanitize_bnf_contents(
		std::vector<std::string> file_contents);
	// given a line taken from a BNF file, 
	void parse_bnf_contents(const std::string bnf_contents);
	std::pair<Symbol, std::size_t> parse_symbol_from_bnf_contents(const std::string bnf_contents, const std::size_t start_index);
	// given a string of the form 
	// <A> ::= <B> <C> | <D> | <E> "**" <G>
	void add_rule(Rules current_rules, const std::string rule_contents);
	

	AbstractSyntaxTreeNode parse_helper(const std::vector<char>& program,
									   int cursor, Symbol symbol);
};

#endif
