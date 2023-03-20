#include "bnf_parser.hpp"

#include <stdexcept>
#include <string>
#include <unordered_set>

namespace BnfParser {

// from https://en.cppreference.com/w/cpp/language/escape
const std::unordered_set<char> escaped_characters = {
	'\'', '\"', '?', '\\', 'a', 'b', 'f', 'n',
	'r',  't',	'v', 'o',  'x', 'u', 'U', 'N'};

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

	if(start_index + BnfParser::bnf_terminal_start.size() <
		   bnf_contents.size() &&
	   bnf_contents.substr(start_index, BnfParser::bnf_terminal_start.size()) ==
		   BnfParser::bnf_terminal_start) {
		symbol_start = BnfParser::bnf_terminal_start;
		symbol_end = BnfParser::bnf_terminal_end;
		is_terminal = true;
	} else if(start_index + BnfParser::bnf_nonterminal_start.size() <
				  bnf_contents.size() &&
			  bnf_contents.substr(start_index,
								  BnfParser::bnf_nonterminal_start.size()) ==
				  BnfParser::bnf_nonterminal_start) {
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

	while(end_position + symbol_end.size() < bnf_contents.size() &&
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

	if(end_position + symbol_end.size() >= bnf_contents.size()) {
		throw std::invalid_argument(
			"No matching " + symbol_end + " at end of symbol " + symbol_start +
			bnf_contents.substr(symbol_contents_start, symbol_contents_size));
	}

	// found symbol_end
	Symbol parsed_symbol = {
		bnf_contents.substr(symbol_contents_start, symbol_contents_size),
		is_terminal};
	return std::make_pair(parsed_symbol, end_position + symbol_end.size() + 1);
}

}  // namespace BnfParser
