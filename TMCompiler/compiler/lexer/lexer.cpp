#include "lexer.hpp"

#include <algorithm>	  // std::max
#include <cstddef>		  // std::ptrdiff_t, std::size_t
#include <iostream>		  // std::endl
#include <regex>		  // std::regex, std::regex_search, std::smatch
#include <stdexcept>	  // std::invalid_argument, std::out_of_range
#include <string>		  // std::string, std::to_string
#include <unordered_map>  // std::unordered_map
#include <utility>		  // std::move, std::pair
#include <vector>		  // std::vector

#include <TMCompiler/compiler/models/grammar_symbol.hpp>  // GrammarSymbol
#include <TMCompiler/compiler/models/rule.hpp>			  // Rule
#include <TMCompiler/compiler/models/token.hpp>			  // Token
#include <TMCompiler/compiler/utils/bnf_parser.hpp>		  // BnfParser
#include <TMCompiler/utils/logger/logger.hpp>			  // LOG

/**
 * Given a list of Rules, convert into a more usable list of token-productions.
 *
 * For instance, parameter "rules" may contain the following:
 * Rule{"token" => "whitespace"}
 * Rule{"token" => "identifier"}
 * Rule{"identifier" => "[a-zA-Z][a-zA-Z0-9]*"
 * Rule{"whitespace" => "\s+"}
 *
 * This function will return the following list:
 * [
 *	  ("whitespace", std::regex("\s+")),
 *	  ("identifier", std::regex("[a-zA-Z][a-zA-Z0-9]*")),
 * ]
 *
 * @param rules: list of Rule objects from a BNF file, containing some
 * productions of non-terminal "token", as well as some productions of
 * non-terminal token-types to their respective terminal regular-expression
 * @return: a list of (token-type, regex) pairs in the order found in rules
 */
auto convert_token_rules_to_regexes(const std::vector<Rule>& rules)
	-> std::vector<std::pair<std::string, std::regex> > {
	// store instances of "whitespace" -> "\s+"
	std::unordered_map<std::string, std::string> token_type_to_regex_string;

	bool found_token_rule = false;
	for(const Rule& rule : rules) {
		if(rule.production.value == "token") {
			found_token_rule = true;
		} else {
			const std::vector<GrammarSymbol> replacement = rule.replacement;
			if(replacement.size() != 1 || !replacement[0].terminal) {
				throw std::invalid_argument(
					"Each rule of lexical grammar must have only one value on "
					"the right-hand-side that is a string literal describing "
					"the regex of the token pattern");
			}

			// found <whitespace> ::= "\s+"; store as "whitespace" -> "\s+"
			token_type_to_regex_string[rule.production.value] =
				replacement.at(0).value;
		}
	}

	if(!found_token_rule) {
		throw std::invalid_argument(
			"Lexical grammar must have one rule with left-hand-side <token>, "
			"with right-hand-side describing the order of operations to check "
			"token type");
	}

	// order (token_type, token_regex) based on order in list of rules
	std::vector<std::pair<std::string, std::regex> > token_regexes;

	// find instances of Rule("token", "whitespace"),
	// Rule("token", "identifier"), ...
	for(const Rule& rule : rules) {
		if(rule.production.value == "token") {
			// add ("whitespace", std::regex("\s+"))
			const std::string token_type = rule.replacement[0].value;

			if(token_type_to_regex_string.find(token_type) ==
			   token_type_to_regex_string.end()) {
				throw std::invalid_argument(
					std::string("Lexical grammar states a token can have type ")
						.append(token_type) +
					", but no production <" + token_type +
					"> ::= \"...\" was found");
			}

			const std::string token_regex =
				token_type_to_regex_string[token_type];
			token_regexes.emplace_back(token_type, std::regex(token_regex));
		}
	}

	return token_regexes;
}

/**
 * Constructor for Lexer class.
 *
 * @param lexical_config_file: file path name of BNF file containing
 * token-productions and token-type productions
 */
Lexer::Lexer(const std::string& lexical_config_file)
	: text{""}, cursor{0}, row{0}, col{0} {
	const std::vector<Rule> rules = BnfParser::parse_rules(lexical_config_file);
	token_regexes = convert_token_rules_to_regexes(rules);
}

/**
 * Setter method for which text to tokenize from
 *
 * For instance, setting text to be "int foo" and later calling
 * get_next_token() might return Token("keyword", "int").
 * This method resets the current read position of text to 0; ie the start of
 * the text.
 *
 * @param text_to_read: string to tokenize from
 */
auto Lexer::set_text(std::string text_to_read) -> void {
	text = std::move(text_to_read);
	cursor = 0;
	row = 0;
	col = 0;
}

/**
 * Checks if there is a valid token to parse from the current position within
 * text.
 *
 * For instance, if text was "int foo", and "int" was already tokenized, the
 * current position would be at 3, the space. has_next_token() would return
 * true because the space would match the "whitespace" token. This does not
 * actually tokenize, and it does not affect the current position.
 *
 * @return: returns true iff at the current position in text, there is a valid
 * token starting at that position.
 */
auto Lexer::has_next_token() const -> bool {
	// if there are no more characters to read, no more tokens
	if(cursor >= text.size()) {
		return false;
	}

	// try matching every token_type's regex at current position
	for(const std::pair<std::string, std::regex>& token_type_regex :
		token_regexes) {
		std::smatch match_result;
		// cursor guaranteed between text.begin() and text.end()
		if(std::regex_search(
			   text.cbegin() + static_cast<std::ptrdiff_t>(cursor),
			   text.cend(),
			   match_result,
			   token_type_regex.second) &&
		   match_result.position() == 0) {
			return true;
		}
	}

	LOG("WARNING") << "Text still has characters, but no token found at line "
				   << 1 + row << ", col " << col << std::endl;

	return false;
}

/**
 * Parses the next valid token from text at the current position.
 *
 * After a token is parsed, the current position is moved to right after
 * the parsed token. A std::out_of_range exception is throw if there is no
 * valid next token to parse.
 *
 * For example, suppose text = "int foo", and get_next_token() has already
 * been called twice and returned the tokens
 * Token("keyword", "int") and Token("whitespace", " ").
 * The current position is at position 4 at character 'f'.
 * Calling get_next_token() would return Token("identifier", "foo").
 *
 * Note: The above example depends on the appropriate configuration in the BNF
 * file. The tokens also have information about the line number and column.
 *
 * @return: a token at the current position of text
 */
auto Lexer::get_next_token() -> Token {
	std::vector<Token> candidate_tokens;

	// try matching every token_type's regex at current position
	for(const std::pair<std::string, std::regex>& token_type_regex :
		token_regexes) {
		std::smatch match_result;
		// cursor guaranteed between text.begin() and text.end()
		if(std::regex_search(
			   text.cbegin() + static_cast<std::ptrdiff_t>(cursor),
			   text.cend(),
			   match_result,
			   token_type_regex.second) &&
		   match_result.position() == 0) {
			// found match!
			const std::string token_type = token_type_regex.first;
			const std::string matched = match_result.str();

			// update column and row position
			std::size_t candidate_row = row;
			std::size_t candidate_col = col;

			for(const char c : matched) {
				if(c == '\n') {
					++candidate_row;
					candidate_col = 0;
				} else {
					++candidate_col;
				}
			}

			candidate_tokens.push_back(
				Token{token_type, matched, candidate_row, candidate_col});
		}
	}

	// out of all possible ones, select the longest
	std::size_t longest_match_size = 0;
	for(const Token& candidate : candidate_tokens) {
		longest_match_size =
			std::max(candidate.value.size(), longest_match_size);
	}

	// break ties by whichever rule came first
	for(const Token& candidate : candidate_tokens) {
		if(candidate.value.size() == longest_match_size) {
			// update current position in text
			row = candidate.program_line_number;
			col = candidate.start_position_of_token_in_program_line;
			cursor += longest_match_size;
			return candidate;
		}
	}

	throw std::out_of_range("No token found at row " + std::to_string(row) +
							", col " + std::to_string(col));
}
