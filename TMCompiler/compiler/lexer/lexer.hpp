#ifndef LEXER_HPP
#define LEXER_HPP

/**
 * The Lexer class implements a scanner that tokenizes a given string into
 * individual tokens / words, based on a list of regular-expressions in a BNF
 * configuration file.
 *
 * The BNF file must have the following structure:
 *		- a "token" production to a list of token-type productions
 *		- a production for every token-type, that leads to one regular
 *expression
 *
 * For instance, the following is a valid BNF configuration:
 * <token> ::= <whitespace>
 *           | <identifier>
 * <whitespace> ::= "\s+"
 * <identifier> ::= "[a-zA-Z][a-zA-Z0-9]*"
 *
 * This tells the lexer that there are two kinds of tokens: whitespace and
 * identifiers, based on the corresponding regular-expression.
 *
 * The lexer will parse text in the order given by the productions of "token"
 *
 * One way to use the lexer is as follows:
 *
 * Lexer lexer{"TMCompiler/config/regex_lexical_grammar.bnf"};
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

#include <regex>	// std::regex
#include <string>	// std::string
#include <utility>	// std::pair
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/token.hpp>	 // Token

class Lexer {
public:
	explicit Lexer(const std::string& lexical_config_file);
	auto set_text(std::string text_to_read) -> void;
	auto has_next_token() const -> bool;
	auto get_next_token() -> Token;

private:
	std::string text;
	unsigned int cursor;
	unsigned int row;
	unsigned int col;
	std::vector<std::pair<std::string, std::regex> > token_regexes;
};

#endif
