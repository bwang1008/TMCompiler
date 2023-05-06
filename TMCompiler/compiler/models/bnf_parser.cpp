#include "bnf_parser.hpp"

#include <cctype>  // std::isspace
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>	// std::pair, std::move

Symbol::Symbol() : name{""} {
}
Symbol::Symbol(std::string name, const bool terminal)
	: name{std::move(name)}, terminal{terminal} {
}
auto Symbol::get_name() -> std::string {
	return name;
}
auto Symbol::get_terminal() -> bool {
	return terminal;
}

namespace BnfParser {

// from https://en.cppreference.com/w/cpp/language/escape
const std::unordered_set<char> escaped_characters = {
	'\'', '\"', '?', '\\', 'a', 'b', 'f', 'n',
	'r',  't',	'v', 'o',  'x', 'u', 'U', 'N'};

/**
 * Finds the next pattern within text starting from pos.
 * If searching for a character that is escaped, find
 * the next unescaped characater.
 *
 * @param text: string to be searching in
 * @param pattern: string to be searching for
 * @param pos: index in text to start searching for
 * @return index at which pattern next exists. std::string::npos
 * if it does not exist
 */
auto find_next_unescaped_string(const std::string& text,
								const std::string& pattern,
								const std::size_t pos) -> std::size_t {
	std::size_t find_pos = pos;
	bool search_current = true;
	do {
		find_pos = text.find(pattern, find_pos + ((search_current) ? 1 : 0));
		search_current = false;
	} while(pattern.size() == 1 && find_pos != std::string::npos &&
			find_pos > 0 && text[find_pos - 1] == '\\' &&
			escaped_characters.find(pattern[0]) != escaped_characters.end());

	return find_pos;
}

/**
 * Returns true if pattern exists in text starting at position pos
 *
 * @param text: string to be searching in
 * @param pattern: string to be searching for
 * @param pos: index in text to start searching for pattern
 * @return true if pattern exists in text at pos; false otherwise
 */
auto text_matches_pattern(const std::string& text, const std::string& pattern,
						  const std::size_t pos) -> bool {
	return pos + pattern.size() <= text.size() &&
		   text.substr(pos, pattern.size()) == pattern;
}

/**
 * Reads a BNF terminal ("abc") or non-terminal (<abc>) from BNF
 *
 * @param bnf_contents: all characters from a BNF file, including newlines
 * @param start_index: index in bnf_contents to start reading symbol
 * @return pair(symbol that was read, and index after end of symbol)
 */
auto parse_symbol(const std::string& bnf_contents,
				  const std::size_t start_index)
	-> std::pair<Symbol, std::size_t> {
	if(start_index >= bnf_contents.size()) {
		throw std::invalid_argument(
			std::string("Trying to read bnf_contents at position ") +
			std::to_string(start_index) +
			std::string(", beyond the length of bnf_contents"));
	}

	// parse depending if starting on " or <
	std::string symbol_start;
	std::string symbol_end;
	bool is_terminal = false;

	if(text_matches_pattern(bnf_contents, BnfParser::bnf_terminal_start,
							start_index)) {
		symbol_start = BnfParser::bnf_terminal_start;
		symbol_end = BnfParser::bnf_terminal_end;
		is_terminal = true;
	} else if(text_matches_pattern(bnf_contents,
								   BnfParser::bnf_nonterminal_start,
								   start_index)) {
		symbol_start = BnfParser::bnf_nonterminal_start;
		symbol_end = BnfParser::bnf_nonterminal_end;
		is_terminal = false;
	} else {
		throw std::invalid_argument(
			"Starting character " + std::string(1, bnf_contents[start_index]) +
			" does not match the BNF symbol delimiters " +
			BnfParser::bnf_terminal_start + " or " +
			BnfParser::bnf_nonterminal_start);
	}

	// "abcdef" <ghi>
	// AB     CD

	// start_index <- A
	// symbol_contents_start <- B
	// end_position <- C
	// return pair(Symbol(abcdef), D)

	const std::size_t symbol_contents_start = start_index + symbol_start.size();
	std::size_t symbol_contents_size = 0;
	std::size_t end_position = symbol_contents_start;

	while(end_position + symbol_end.size() <= bnf_contents.size() &&
		  bnf_contents.substr(end_position, symbol_end.size()) != symbol_end) {
		if(bnf_contents[end_position] == '\\' &&
		   end_position + 1 < bnf_contents.size() &&
		   escaped_characters.find(bnf_contents[end_position + 1]) !=
			   escaped_characters.end()) {
			symbol_contents_size += 2;
			end_position += 2;
		} else {
			symbol_contents_size += 1;
			end_position += 1;
		}
	}

	if(end_position + symbol_end.size() > bnf_contents.size()) {
		throw std::invalid_argument(
			"No matching " + symbol_end + " at end of symbol " + symbol_start +
			bnf_contents.substr(symbol_contents_start, symbol_contents_size));
	}

	// found symbol_end
	Symbol parsed_symbol = {
		bnf_contents.substr(symbol_contents_start, symbol_contents_size),
		is_terminal};
	return std::make_pair(parsed_symbol, end_position + symbol_end.size());
}

/**
 * Reads a BNF file and returns the rules listed as a Rules object
 *
 * @param bnf_file_stream: BNF file to read from
 * @return Rules object representing the replacement rules in the BNF file
 */
auto parse_rules(std::ifstream& bnf_file_stream) -> Rules {
	if(!bnf_file_stream.is_open()) {
		throw std::invalid_argument("Unable to open BNF file");
	}

	std::string bnf_contents;

	try {
		std::string line;
		while(std::getline(bnf_file_stream, line)) {
			bnf_contents += line;
			bnf_contents += "\n";
		}
	} catch(const std::ifstream::failure& ios_exception) {
		bnf_file_stream.close();
		throw ios_exception;
	}

	bnf_file_stream.close();

	return parse_rules(bnf_contents);
}

/**
 * Converts a BNF file's contents into different tokens / words:
 * <A> ::= <B> | "C" turns into ["<A>", "::=", "<B>", "|", "\"C\""]
 * Comments (# til end of line) are removed
 *
 * @param bnf_contents: all characters from a BNF file, including newlines
 * @return list of words from the BNF file
 */
auto tokenize(const std::string& bnf_contents) -> std::vector<std::string> {
	std::vector<std::string> tokens;
	std::size_t curr_index = 0;

	while(curr_index < bnf_contents.size()) {
		if(std::isspace(bnf_contents[curr_index]) != 0) {
			// ignore whitespace
			curr_index++;
		} else if(text_matches_pattern(
					  bnf_contents, BnfParser::bnf_comment_start, curr_index)) {
			// read comment until end of line
			std::size_t newline_position =
				bnf_contents.find('\n', curr_index + 1);
			if(newline_position == std::string::npos) {
				newline_position = bnf_contents.size();
			}
			curr_index = 1 + newline_position;
		} else if(text_matches_pattern(bnf_contents,
									   BnfParser::bnf_replacement_separation,
									   curr_index)) {
			// found "::="
			tokens.push_back(BnfParser::bnf_replacement_separation);
			curr_index += BnfParser::bnf_replacement_separation.size();
		} else if(text_matches_pattern(bnf_contents, BnfParser::bnf_choice,
									   curr_index)) {
			// found "|"
			tokens.push_back(BnfParser::bnf_choice);
			curr_index += BnfParser::bnf_choice.size();
		} else if(text_matches_pattern(bnf_contents,
									   BnfParser::bnf_terminal_start,
									   curr_index)) {
			// found "\""
			const std::size_t end_position = find_next_unescaped_string(
				bnf_contents, BnfParser::bnf_terminal_end, curr_index + 1);
			const std::size_t next_position =
				end_position + BnfParser::bnf_terminal_end.size();
			tokens.push_back(
				bnf_contents.substr(curr_index, next_position - curr_index));
			curr_index = next_position;
		} else if(text_matches_pattern(bnf_contents,
									   BnfParser::bnf_nonterminal_start,
									   curr_index)) {
			// found "<"
			const std::size_t end_position = find_next_unescaped_string(
				bnf_contents, BnfParser::bnf_nonterminal_end, curr_index + 1);
			const std::size_t next_position =
				end_position + BnfParser::bnf_nonterminal_end.size();
			tokens.push_back(
				bnf_contents.substr(curr_index, next_position - curr_index));
			curr_index = next_position;
		} else {
			throw std::invalid_argument(
				"Unknown character while tokenizing: " +
				std::string(1, bnf_contents[curr_index]));
		}
	}

	return tokens;
}

/**
 * Returns rules listed in the string containing BNF contents
 *
 * @param bnf_contents lines of BNf file concatenated by newlines
 * @return Rules object representing the replacement rules in the BNF file
 */
auto parse_rules(const std::string& bnf_contents) -> Rules {
	// tokenize string into list of words
	const std::vector<std::string> tokens = tokenize(bnf_contents);

	// separate out into different rules:
	// there is one nonterminal symbol before a ::=
	Rules rules;
	Symbol current_left;
	ReplacementAlternatives current_replacements;

	for(std::size_t i = 0; i < tokens.size(); ++i) {
		const std::string token = tokens[i];
		if(i + 1 < tokens.size() &&
		   tokens[i + 1] == BnfParser::bnf_replacement_separation) {
			// current rule ends; new rule started
			if(!current_replacements.empty()) {
				rules[current_left.get_name()].insert(
					rules[current_left.get_name()].end(),
					current_replacements.begin(), current_replacements.end());
			}

			current_left = parse_symbol(token, 0).first;
			current_replacements.clear();
		} else if(token == BnfParser::bnf_choice) {
			// start new rule replacement
			current_replacements.push_back(std::vector<Symbol>());
		} else if(token != BnfParser::bnf_replacement_separation) {
			// add to last rule replacement if see a symbol
			Symbol current_symbol = parse_symbol(token, 0).first;

			if(current_replacements.empty()) {
				current_replacements.push_back(std::vector<Symbol>());
			}

			current_replacements.back().push_back(current_symbol);
		}
	}

	// add last rule
	if(!current_replacements.empty()) {
		rules[current_left.get_name()].insert(
			rules[current_left.get_name()].end(), current_replacements.begin(),
			current_replacements.end());
	}
	return rules;
}

}  // namespace BnfParser
