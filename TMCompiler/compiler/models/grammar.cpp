#include "grammar.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <iostream>

Grammar::Grammar(const std::string& bnf_file_name) {
	std::ifstream bnf_file(bnf_file_name);
	if(!bnf_file.is_open()) {
		throw std::invalid_argument("Unable to open grammar file " +
									bnf_file_name);
	}

	// read file and store contents
	std::vector<std::string> file_contents;
	std::string line;
	while(std::getline(bnf_file, line)) {
		file_contents.push_back(line);
	}

	bnf_file.close();

	// remove comments and empty lines
	std::vector<std::string> sanitized_file_contents = sanitize_bnf_contents(file_contents);

	std::cout << "Sanitized BNF file:" << std::endl;
	for(std::size_t i = 0; i < sanitized_file_contents.size(); ++i) {
		std::cout << 1 + i << ": " << sanitized_file_contents[i] << std::endl;
	}
	std::cout << "------------------------------" << std::endl;

	// 

}

Grammar::Grammar(std::unordered_map<Symbol, std::vector<std::vector<Symbol> > > rules, std::string default_start) {
	this->rules = rules;
	this->default_start = default_start;
}

// LR parsing? Earley parsing?
AbstractSyntaxTreeNode Grammar::parse_helper(const std::vector<char>& program,
											int cursor, Symbol symbol) {
}

AbstractSyntaxTree Grammar::parse(const std::vector<char>& program) {
	AbstractSyntaxTree ast{
		// parse_helper(program, 0, symbol_mapping[default_start])
	};
	return ast;
}

/**
 * Given a string s, return a copy of the string with surrounding whitespace removed
 * Ex: trim("   abc def   ") -> "abc def"
 */
std::string trim(const std::string s) {
	const std::unordered_set<char> whitespace {' ', '\n', '\r', '\t'};
	std::size_t left, right;

	std::size_t currPointer = 0;
	while(currPointer < s.size() && whitespace.find(s[currPointer]) != whitespace.end()) {
		++currPointer;
		left = currPointer;
	}
	
	while(currPointer < s.size()) {
		if(whitespace.find(s[currPointer]) == whitespace.end()) {
			++currPointer;
			right = currPointer;
		}
	}
	
	return s.substr(left, right);
}

/**
 * Given lines of a BNF file, remove comments from "#" and empty lines
 */
std::vector<std::string> Grammar::sanitize_bnf_contents(
	std::vector<std::string> file_contents) {
	std::vector<std::string> sanitized_lines;

	for(const std::string line : file_contents) {
		// go through each character until you find a "#"
		// skip middle of quoted lines like "abc" and angled <abc>
		std::size_t endpoint = 0;
		while(endpoint < line.size()) {
			if(line[endpoint] == Grammar::bnf_comment_start) {
				// comments start with a # and go to end of the line
				break;
			} else if(line[endpoint] == Grammar::bnf_terminal_start) {
				// if see a quotation ", get the opposite
				// ignore if preceded by backslash "\"
				std::size_t preceding = endpoint;
				++endpoint;
				while(endpoint < line.size()) {
					if(line[endpoint] == Grammar::bnf_terminal_end && line[preceding] != '\\') {
						break;
					}

					++preceding;
					++endpoint;
				}
			} else if(line[endpoint] == Grammar::bnf_nonterminal_start) {
				endpoint =
					line.find(Grammar::bnf_nonterminal_end, endpoint + 1);
			} else {
				++endpoint;
			}
		}

		if(endpoint == std::string::npos) {
			throw std::invalid_argument("Syntax error in BNF file: un-matched " + Grammar::bnf_terminal_start + " or " + Grammar::bnf_nonterminal_start + " in line " + line);
		}

		std::string sanitized_line = trim(line.substr(0, line));
		if(sanitized_line.size() > 0) {
			sanitized_lines.push_back(sanitized_line);
		}
	}

	return sanitized_lines;
}

/**
 * Given a string of the form
 * <A> ::= <B> <C> | <D> | <E> "**" <G> ...
 * add to our map of rules
 */
void Grammar::add_rule(std::unordered_map<Symbol, std::vector<std::vector<Symbol> > > current_rules, const std::string rule_contents) {
	std::size_t replacement_operator_location = rule_contents.find(Grammar::bnf_replacement_separation);

	if(replacement_operator_location == std::string::npos) {
		throw std::invalid_argument("String containing a rule does not contain the separator " + Grammar::bnf_replacement_separation + " in line " + rule_contents);
	}

	// split into left and right side of rule, based on location of "::="
	std::string left_hand_side = trim(rule_contents.substr(0, replacement_operator_location));
	std::string right_hand_side = trim(rule_contents.substr(replacement_operator_location + Grammar::bnf_replacement_separation.size()));

}
