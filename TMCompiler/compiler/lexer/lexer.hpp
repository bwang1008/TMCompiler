#ifndef LEXER_HPP
#define LEXER_HPP

#include <regex>	// std::regex
#include <string>	// std::string
#include <utility>	// std::pair
#include <vector>	// std::vector

#include <TMCompiler/compiler/models/token.hpp>	 // Token

class Lexer {
public:
	explicit Lexer(const std::string& lexical_config_file);
	auto set_text(std::string text_to_read) -> void;
	auto has_next_token() -> bool;
	auto get_next_token() -> Token;

private:
	std::string text;
	unsigned int cursor;
	unsigned int row;
	unsigned int col;
	std::vector<std::pair<std::string, std::regex> > token_regexes;
};

#endif
