#ifndef LEXER_HPP
#define LEXER_HPP

/**
 * The Lexer class implements a scanner that tokenizes a given string into
 * individual tokens / words, based on a list of regular-expressions.
 *
 * For instance, given the following as input,
 * ("whitespace", std::regex("\\s+")),
 * ("identifier", std::regex("[a-zA-Z][a-zA-Z0-9]*")
 *
 * This tells the lexer that there are two kinds of tokens: whitespace and
 * identifiers, based on the corresponding regular-expression.
 *
 * The lexer will parse text in the order given.
 *
 * One way to use the lexer is as follows:
 *
 * std::vector<std::pair<std::string, std::regex>> regexs;
 * regexs.emplace_back("whitespace", std::regex("\\s+"));
 * regexs.emplace_back("identifier", std::regex("[a-zA-Z][a-zA-Z0-9]*"));
 * Lexer lexer{regexs};
 * lexer.set_text("int foo");
 * while(lexer.has_next_token()) {
 *		Token token = lexer.get_next_token();
 * }
 *
 * which will parse out the following tokens:
 * Token{"keyword", "int", 0, 0};
 * Token{"whitespace", " ", 0, 3};
 * Token{"identifier", "foo", 0, 4};
 */

#include <cstddef>	// std::size_t
#include <regex>	// std::regex
#include <string>	// std::string
#include <utility>	// std::pair
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/token.hpp>	 // Token

class Lexer {
public:
	explicit Lexer(
		std::vector<std::pair<std::string, std::regex>> _token_regexes);
	auto set_text(std::string text_to_read) -> void;
	[[nodiscard]] auto has_next_token() const -> bool;
	auto get_next_token() -> Token;

private:
	std::string text;	 // text to parse from
	std::size_t cursor;	 // current position in text
	std::size_t row;
	std::size_t col;
	// list of token type to regex, such as
	// [
	//	  ("whitespace", std::regex("\s+")),
	//	  ("identifier", std::regex("[a-zA-Z][a-zA-Z0-9]//")),
	// ]
	std::vector<std::pair<std::string, std::regex>> token_regexes;
};

#endif
